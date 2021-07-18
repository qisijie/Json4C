// Json4C.c: 定义应用程序的入口点。
//
#include "Json4C.h"


static void* (*Json4C_malloc)(size_t sz) = malloc;
static void (*Json4C_free)(void* ptr) = free;

static const char* endPosition;

char* Json4C_GetEndPosition()
{
    return endPosition;
}

Json4C* Json4C_New_Item()
{
    Json4C* node = (Json4C*)Json4C_malloc(sizeof(Json4C));

        if (node)
            memset(node, 0, sizeof(Json4C));
        return node;
}

const char* skip(const char* in)
{
    while (in && *in && (unsigned char)*in <= 32)
        in++;
    return in;
}

void Json4C_Detele(Json4C* c) 
{
    Json4C* next;
    while (c)
    {
        next = c->next;
        if (c->child)
            Json4C_Detele(c->child);
        if (c->valueString)
            Json4C_free(c->valueString);
        if(c->string)
            Json4C_free(c->string);
        Json4C_free(c);
        c = next;

    }
}

const char* parse_string(Json4C* item, const char* str)
{
    const char* ptr = str + 1;
    char* ptr2;
    char* out;
    int lenth = 0;

    if (*str != '\"')
    {
        endPosition = str;
        return NULL;
    }

    while (*ptr != '\"' && *ptr && ++lenth)
    {
        if (*ptr++ == '\\')
            ptr++;
    }

    out = (char*)Json4C_malloc(lenth + 1);
    if (!out)
        return NULL;

    ptr = str + 1;
    ptr2 = out;

    while (*ptr != '\"' && *ptr)
    {
        if (*ptr != '\\')
            *ptr2++ = *ptr++;
        else
        {
            ptr++;
            switch (*ptr)
            {
                case 'a':
                    *ptr2++ = '\a';
                    break;
                case 'b':
                    *ptr2++ = '\b';
                    break;
                case 'f':
                    *ptr2++ = '\f';
                    break;
                case 'n':
                    *ptr2++ = '\n';
                    break;
                case 'r':
                    *ptr2++ = '\r';
                    break;
                case 't':
                    *ptr2++ = '\t';
                    break;
                case 'v':
                    *ptr2++ = '\v';
                    break;
                default:
                    *ptr2++ = *ptr;
                    break;
            }
            ptr++;
        }
    }

    *ptr2 = 0;

    if (*ptr == '\"')
        ptr++;

    item->valueString = out;
    item->tpye = Json4C_String;

    return ptr;
}

const char* parse_number(Json4C* item, const char* num)
{
    double n = 0;//最终的数字
    int sign = 1;//正负号
    int signSubScale = 1;
    int scale = 0;
    int subscale = 1;

    if (*num == '-')
    {
        sign = -1;
        num++;
    }

    if (*num == '0')
        num++;

    if (*num >= '0' && *num <= '9')
    {
        //eg.123
        do 
        {
            n = (n * 10.0) + (*num++ - '0');
        } while (*num >= '0' && *num <= '9');
    }

    if (*num == '.' && num[1] >= '0' && num[1] <= '9')
    {
        //eg.123.456
        num++;
        do
        {
            n = (n * 10.0) + (*num++ - '0');
            scale--;
        } while (*num >= '0' && *num <= '9');
    }

    if (*num == 'e' || *num == 'E')
    {
        //eg.123e456 123e-456
        num++;
        if (*num == '+')
            num++;
        else if (*num == '-')
        {
            signSubScale = -1;
            num++;
        }
        do
        {
            subscale = (subscale * 10.0) + (*num++ - '0');
        } while (*num >= '0' && *num <= '9');
    }

    n = sign * n * pow(10.0, (scale + signSubScale * subscale));
    item->valueDouble = n;
    item->valueInt = (int)n;
    item->tpye = Json4C_Number;
    return num;
}

const char* parse_array(Json4C* item, const char* value)
{
    //eg.["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]
    Json4C* child;
    if (*value != '[')
    {
        endPosition = value;
        return NULL;
    }
    item->tpye = Json4C_Array;

    value = skip(value + 1);
    if (*value == ']')//防止[]的出现
        return value + 1;

    item->child = child = Json4C_New_Item();
    if (!item->child)
        return NULL;
    
    value = skip(parse_value(child,skip(value)));
    if (!value)
        return NULL;

    while (*value == ',')
    {
        Json4C* new_item;
        if (!(new_item = Json4C_New_Item()))
            return NULL;
        child->next = new_item;
        new_item->prev = child;
        child = new_item;

        value = skip(parse_value(child, skip(value + 1)));
        if (!value)
            return NULL;
    }

    if (*value == ']')
        return value + 1;

    endPosition = value;
    return NULL;
}

const char* parse_object(Json4C* item, const char* value) 
{
    Json4C* child;
    if (*value != '{')
    {
        endPosition = value;
        return NULL;
    }

    item->tpye = Json4C_Object;
    value = skip(value + 1);
    if (*value == '}')
        return value + 1;

    item->child = child = Json4C_New_Item();
    if (!item->child)
        return NULL;

    value = skip(parse_string(child, skip(value)));
    if (!value)
        return NULL;

    child->string = child->valueString;
    child->valueString = NULL;

    if (*value != ':')
    {
        endPosition = value;
        return NULL;
    }

    value = skip(parse_value(child, skip(value + 1)));
    if (!value)
        return NULL;

    while (*value == ',')
    {
        Json4C* new_item;
        if (!(new_item = Json4C_New_Item()))
            return NULL;
        child->next = new_item;
        new_item->prev = child;
        child = new_item;

        value = skip(parse_string(child, skip(value + 1)));
        if (!value)
            return NULL;
    

        child->string = child->valueString;
        child->valueString = NULL;

        if (*value != ':')
        {
            endPosition = value;
            return NULL;
        }

        value = skip(parse_value(child, skip(value + 1)));
        if (!value)
            return NULL;
    }

    if (*value == '}')
        return value + 1;

    endPosition = value;
    return NULL;
}

const char* parse_value(Json4C* item, const char* value)
{
    if (!value)
        return NULL;

    if (!strncmp(value, "false", 5))
    {
        item->tpye = Json4C_False;
        return value +  5;
    }

    if (!strncmp(value, "true", 4))
    {
        item->tpye = Json4C_True;
        item->valueInt = 1;
        return value + 4;
    }

    if (!strncmp(value, "null", 4))
    {
        item->tpye = Json4C_Null;
        return value + 4;
    }

    if (*value == '\"')
        return parse_string(item, value);

    if (*value == '-' || (*value >= '0' && *value <= '9'))
        return parse_number(item, value);

    if (*value == '[')
        return parse_array(item, value);

    if (*value == '{')
        return parse_object(item, value);

    endPosition = value;
    return NULL;
}

Json4C* Json4C_Parse(const char* value)
{
    return Json4C_ParseWithOptions(value, 0, 0);
}

Json4C* Json4C_ParseWithOptions(const char* value,const char** return_parse_end,int require_null_terminated)
{
    const char* end = NULL;
    Json4C* c = Json4C_New_Item();
    endPosition = NULL;
    if (!c)
        return NULL;

    end = parse_value(c,skip(value));
    if (!end)
    {
        Json4C_Detele(c);
        return NULL;
    }
    
    if (require_null_terminated)
    {
        end = skip(end);
        if (*end)
        {
            Json4C_Detele(c);
            endPosition = end;
            return NULL;
        }
    }

    if (return_parse_end) 
    {
        *return_parse_end = end;
    }

    return c;
}

//---------------------------------------------

char* print_value(Json4C* item, int deepth, int fmt)
{
    char* out = NULL;

    if (!item)
        return NULL;

    switch ((item->tpye) & 255)
    {
    case Json4C_False:
        out = Json4C_strdup("false");
        break;
    case Json4C_True:
        out = Json4C_strdup("true");
        break;
    case Json4C_Null:
        out = Json4C_strdup("null");
        break;
    case Json4C_Number:
        out = print_number(item);
        break;
    case Json4C_String:
        out = print_string(item,0);
        break;
    case Json4C_Array:
        out = print_array(item,deepth,fmt);
        break;
    case Json4C_Object:
        out = print_object(item, deepth, fmt);
        break;
    default:
        break;
    }
}

char* Json4C_strdup(const char* str)
{
    char* copy = NULL;
    size_t lenth;

    lenth = strlen(str);
    copy = (char*)Json4C_malloc(lenth + 1);
    if (!copy)
        return NULL;
    memcpy(copy, str, lenth + 1);
    return copy;
}

char* print_number(Json4C* item)
{
    char* str = NULL;
    double dbl = item->valueDouble;
    if (dbl == 0)
    {
        str = (char*)Json4C_malloc(2);
        if (str)
            strcpy(str, "0");
    }
    //整数
    else if ((fabs((double)(item->valueInt) - dbl) <= DBL_EPSILON) && dbl <= INT_MAX && dbl >= INT_MAX)
    {
        str = (char*)Json4C_malloc(21);
        if (str)
            sprintf(str, "%d", item->valueInt);
    }
    //浮点数
    else
    {
        str = (char*)Json4C_malloc(64);
        if (str)
        {
            //123.456
            if (fabs(fabs(dbl) - dbl) <= DBL_EPSILON && fabs(dbl) < 1.0e60)
                sprintf(str, "%.0f", dbl);
            else if (fabs(dbl) < 1.0e-6 || fabs(1.0e9))
                sprintf(str, "%e", dbl);
            else
                sprintf(str, "%f", dbl);
        }
    }
    return str;
}

char* print_string(Json4C* item, int isName) 
{
    char* str = (isName > 1) ? item->valueString : item->string;
    char* ptr, * ptr2, * out;
    unsigned char token;
    int flag = 0,lenth = 0;
    for (ptr = str; *ptr; ptr++)
    {
        flag |= (*ptr > 0 && *ptr < 32) || (*ptr == '\"') || (*ptr == '\\') ? 1 : 0;
    }

    if (!flag) 
    {
        lenth = ptr - str;
        out = (char*)Json4C_malloc(lenth + 2 + 1);//2给"",1给\0
        if (!out)
            return NULL;

        ptr2 = out;
        *ptr2++ = '\"';
        strcpy(ptr2, str);
        ptr2[lenth] = '\"';
        ptr2[lenth + 1] = '\0';
        return out;
    }

    if (!str)
    {
        out = (char*)Json4C_malloc(3);
        if (!out)
            return NULL;
        strcpy(out, "\"\"");
        return out;
    }
    
    ptr = str;
    while ((token = *ptr) && ++lenth)
    {
        //printf("i am in while");
        if (strchr("\a\b\f\n\r\t\v\\\'\"\?", token))
            lenth++;
        else if (token < 32)
        {
            lenth += 5;
            ptr++;
        }
    }

    out = (char*)Json4C_malloc(lenth + 3);
    if (!out)
        return NULL;

    ptr2 = out;
    ptr = str;
    *ptr2++ = '\"';

    while (*ptr)
    {
        if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\')
        {
            *ptr2 = *ptr++;
        }
        else
        {
            *ptr2 = '\\';
            switch (token = *ptr)
            {
            case '\a':
                *ptr2++ = '\a';
                 break;
            case '\b':
                *ptr2++ = '\b';
                break;
            case '\f':
                *ptr2++ = '\f';
                break;
            case '\n':
                *ptr2++ = '\n';
                break;
            case '\r':
                *ptr2++ = '\r';
                break;
            case '\t':
                *ptr2++ = '\t';
                break;
            case '\v':
                *ptr2++ = '\v';
                break;
            case '\\':
                *ptr2++ = '\\';
                break;
            case '\'':
                *ptr2++ = '\'';
                break;
            case '\"':
                *ptr2++ = '\"';
                break;
            case '\?':
                *ptr2++ = '\?';
                break;
            default:
                sprintf(str, "u%04x", token);
                ptr2++;
                break;
            }
        }
    }

    *ptr2++ = '\"';
    *ptr2++ = '\0';

    return out;
}

char* print_array(Json4C* item, int deepth, int fmt)
{
    char** entries;//char *entries[];
    char* out = NULL, * ptr, * ret;
    int lenth = 5, i = 0, isFail = 0, templenth = 0;//"[]"\0 共5个

    Json4C* child = item->child;
    int numEntries = 0;

    while (child) 
    {
        //广度优先
        numEntries++;
        child = child->next;
    }

    if (!numEntries)
    {
        out = (char*)Json4C_malloc(3);//[]\0 共3个
        if (out)
            strcpy(out, "[]");
        return 0;
    }

    entries = (char**)Json4C_malloc(numEntries * sizeof(char*));
    if (!entries)
        return NULL;
    memset(entries, 0, numEntries * sizeof(char*));
    child = item->child;
    while (child) 
    {
        ret = print_value(child, deepth + 1, fmt);
        entries[i++] = ret;
        if (ret)
            lenth += strlen(ret) + 2 + (fmt ? 1 : 0);
        else
        {
            isFail = 1;
        }
        child = child->next;
    }

    if (!isFail)
        out = (char*)Json4C_malloc(lenth);

    if (!out)
        isFail = 1;

    if (isFail) 
    {
        for (i = 0; i < numEntries; i++) 
        {
            if (entries[i])
                Json4C_free(entries[i]);
        }
        Json4C_free(entries);

        return NULL;
    }

    *out = '[';
    ptr = out + 1;
    *ptr = '\0';
    for (int i = 0; i < numEntries; i++)
    {
        templenth = strlen(entries[i]);
        memcpy(ptr, entries[i], templenth);
        ptr += templenth;

        if (i != numEntries - 1)
        {
            *ptr++ = ',';
            if (fmt)
                *ptr++ = ' ';
            *ptr = '\0';
        }

        Json4C_free(entries[i]);
    }
    Json4C_free(entries);

    *ptr++ = ']';
    *ptr++ = '\0';

    return out;
}

char* print_object(Json4C* item, int deepth, int fmt)
{
    char** entries = NULL, ** names = NULL;
    char* out = NULL, * ptr, * ret, * str;
    int lenth = 7, i = 0, isFail = 0, templenth = 0;//"{\n\t}"\0 共7个

    Json4C* child = item->child;
    int numEntries = 0;

    while (child) 
    {
        numEntries++;
        child = child->next;
    }

    if (!numEntries)
    {
        out = (char*)Json4C_malloc(fmt ? deepth + 4 : 3);
        if (!out)
            return NULL;

        ptr = out;
        *ptr++ = '{';
        if (fmt)
        {
            *ptr++ = '\n';
            for (i = 0; i < deepth - 1; i++) 
            {
                *ptr++ = '\t';
            }
        }

        *ptr++ = '}';
        *ptr++ = '\0';

        return out;
    }

    entries = (char**)Json4C_malloc(numEntries * sizeof(char*));
    if (!entries)
        return NULL;
    names = (char**)Json4C_malloc(numEntries * sizeof(char*));
    if (!names)
    {
        Json4C_free(entries);
        return NULL;
    }
        

    memset(entries, 0, numEntries * sizeof(char*));
    memset(names, 0, numEntries * sizeof(char*));

    child = item->child;
    deepth++;
    if (fmt)
        lenth += deepth;

    while (child)
    {
        names[i] = str = print_string(child,1);
        entries[i++] = ret = print_value(child, deepth, fmt);
        if (str && ret)
            lenth += strlen(ret) + strlen(str) + 2 + (fmt ? 2 + deepth : 0);//"" 共2个
        else
            isFail = 1;

        child = child->next;
    }

    if (!isFail)
        out = (char*)Json4C_malloc(lenth);
    if (!out)
        isFail = 1;
    if (isFail)
    {
        for (i = 0; i < numEntries; i++)
        {
            if (names[i])
                Json4C_free(names[i]);
            if (entries[i])
                Json4C_free(entries[i]);
        }

        Json4C_free(names);
        Json4C_free(entries);

        return NULL;
    }

    *out = '{';
    ptr = out + 1;
    if (fmt)
        *ptr++ = '\n';
    *ptr = '\0';

    for (i = 0; i < numEntries; i++)
    {
        if (fmt)
            for (int j = 0; j < deepth; j++)
                *ptr++ = '\t';

        templenth = strlen(names[i]);
        memcpy(ptr, names[i], templenth);
        ptr += templenth;
        *ptr++ = ':';
        if (fmt)
            *ptr++ = '\t';

        strcpy(ptr, entries[i]);
        ptr += strlen(entries[i]);

        if (i != numEntries - 1)
            *ptr++ = ',';
        if (fmt)
            *ptr++ = '\n';
        *ptr = '\0';

        Json4C_free(names[i]);
        Json4C_free(entries[i]);
    }

    Json4C_free(names);
    Json4C_free(entries);

    if (fmt)
    {
        for (i = 0; i < deepth - 1; i++)
        {
            *ptr++ = '\t';
        }
    }

    *ptr++ = '}';
    *ptr++ = '\0';

    return out;
}

char* Json4C_Print(Json4C* item) 
{
    return print_value(item,0,1);
}

//---------------------------------------------

Json4C* Json4C_CreateObject(void) 
{
    Json4C* item = Json4C_New_Item();
    if (item)
        item->tpye = Json4C_Object;
    return item;
}

Json4C* Json4C_CreateString(const char* str)
{
    Json4C* item = Json4C_New_Item();
    if (item)
    {
        item->tpye = Json4C_String;
        item->valueString = Json4C_strdup(str);
    }
    return item;
}

Json4C* Json4C_CreateNumber(double num)
{
    Json4C* item = Json4C_New_Item();
    if (item)
    {
        item->tpye = Json4C_Number;
        item->valueDouble = num;
        item->valueInt = (int)num;
    }
    return item;
}

Json4C* Json4C_CreateArray(void) 
{
    Json4C* item = Json4C_New_Item();
    if (item)
        item->tpye = Json4C_Array;
    return item;
}

Json4C* Json4C_CreateBool(int b) 
{
    Json4C* item = Json4C_New_Item();
    if (item)
        item->tpye = b ? Json4C_True : Json4C_False;
    return item;
}

Json4C* Json4C_CreateTrue(void) 
{
    Json4C* item = Json4C_New_Item();
    if (item)
        item->tpye = Json4C_True;
    return item;
}

Json4C* Json4C_CreateFalse(void) 
{
    Json4C* item = Json4C_New_Item();
    if (item)
        item->tpye = Json4C_False;
    return item;
}

Json4C* Json4C_CreateNull(void)
{
    Json4C* item = Json4C_New_Item();
    if (item)
        item->tpye = Json4C_Null;
    return item;
}

void suffix_object(Json4C* prev, Json4C* item) 
{
    prev->next = item;
    item->prev = prev;
}

void Json4C_AddItemToObject(Json4C* object, const char* str, Json4C* item)
{
    if (!item)
        return;
    if (!item->string)
        Json4C_free(item->string);
    item->string = Json4C_strdup(str);

    Json4C_AddItemToArray(object, item);
}

void Json4C_AddItemToArray(Json4C* arr, Json4C* item)
{
    Json4C* c = arr->child;
    if (!item)
        return;
    if (!c)
        arr->child = item;
    else 
    {
        while (c && c->next)
            c = c->next;
        suffix_object(c, item);
    }
}

//---------------------------------------------

Json4C* Json4C_DetachItemFromArray(Json4C* arr, int which)
{
    Json4C* c = arr->child;
    while (c && which>0)
    {
        c = c->next;
        which--;
    }

    if (!c)
        return NULL;
    if (c->prev)
        c->prev->next = c->next;
    if (c->next)
        c->next->prev = c->prev;
    if (c == arr->child)
        arr->child = c->next;
    c->prev = c->next = NULL;

    return c;
}

void Json4C_DeteleItemFromArray(Json4C* arr, int which)
{
    Json4C_Detele(Json4C_DetachItemFromArray(arr, which));
}

Json4C* Json4C_DetachItemFromObject(Json4C* object, const char* str)
{
    int i = 0;
    Json4C* c = object->child;
    while(c && Json4C_strcasecmp(c->string,str))
    {
        i++;
        c = c->next;
    }

    if (c)
    {
        return Json4C_DetachItemFromArray(object, i);
    }

    return NULL;
}

void Json4C_DeteleItemFromObject(Json4C* object, int which)
{
    Json4C_Detele(Json4C_DetachItemFromObject(object, which));
}

int Json4C_strcasecmp(const char* str1, const char* str2)
{
    int ret;
    if (!str1)
        return (str1 == str2) ? NULL : 1;
    if (!str2)
        return 1;

    for (; tolower(str1) == tolower(str2); ++str1, ++str2)
    {
        if (*str1 == 0)
            return 0;
    }
    ret = tolower(*(const unsigned char*)str1) - tolower(*(const unsigned char*)str2);
    return ret;
}

//---------------------------------------------

void Json4C_InsertItemInArray(Json4C* arr, int which, Json4C* newItem)
{
    Json4C* c = arr->child;
    while (c && which > 0)
    {
        c = c->next;
        which--;
    }

    if (!c)
    {
        Json4C_AddItemToArray(arr, newItem);
        return;
    }

    newItem->next = c;
    newItem->prev = c->prev;
    c->prev = newItem;

    if (c == arr->child)
        arr->child = newItem;
    else
        newItem->prev->next = newItem;
}

void Json4C_ReplaceItemInArray(Json4C* arr, int which, Json4C* newItem)
{
    Json4C* c = arr->child;
    while (c && which > 0)
    {
        c = c->next;
        which--;
    }

    if (!c)
        return;

    newItem->next = c->next;
    newItem->prev = c->prev;

    if (newItem->next)
        newItem->next->prev = newItem;
    if (c == arr->child)
        arr->child = newItem;
    else
        newItem->prev->next = newItem;

    c->next = c->prev = NULL;

    Json4C_Detele(c);
}

void Json4C_ReplaceItemObject(Json4C* object, const char* str, Json4C* newItem)
{
    int i = 0;
    Json4C* c = object->child;
    while (c && Json4C_strcasecmp(c->string, str))
    {
        i++;
        c = c->next;
    }

    if (c) 
    {
        newItem->string = Json4C_strdup(str);
        Json4C_ReplaceItemInArray(object, i, newItem);
    }
}