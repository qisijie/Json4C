// Json4C.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#ifndef _JSON4C_H_
#define _JSON4C_H_


/*
{
    "name": "Jack (\"Bee\") Nimble",
    "format": {
        "type":       "rect",
        "width":      1920,
        "height":     1080,
        "interlace":  false,
        "frame rate": 24
    }
}

root -> child(name)
              format -> child(type)
                              width
                              height
                              interlace
                              frame rate
*/

#define Json4C_False   0
#define Json4C_True    1
#define Json4C_Null    2
#define Json4C_Number  3
#define Json4C_String  4
#define Json4C_Array   5
#define Json4C_Object  6

/*
功能:
    创建一个string值为name的Json4C_False节点,并添加到object
*/
#define Json4C_AddFalseToObject(object,name) \
    Json4C_AddItemToObject(object,name,Json4C_CreateFalse())

/*
功能:
    创建一个string值为name的Json4C_True节点,并添加到object
*/
#define Json4C_AddTrueToObject(object,name) \
    Json4C_AddItemToObject(object,name,Json4C_CreateTrue())

/*
功能:
    创建一个string值为name的Json4C_Bool节点,并添加到object
*/
#define Json4C_AddBoolToObject(object,name,boolnumber) \
    Json4C_AddItemToObject(object,name,Json4C_CreateBool(boolnumber))

/*
功能:
    创建一个string值为name的Json4C_NULL节点,并添加到object
*/
#define Json4C_AddNullToObject(object,name) \
    Json4C_AddItemToObject(object,name,Json4C_CreateNull())

/*
功能:
    创建一个string值为name的Json4C_Number节点,并添加到object
*/
#define Json4C_AddNumberToObject(object,name,n) \
    Json4C_AddItemToObject(object,name,Json4C_CreateNumber(n))

/*
功能:
    创建一个string值为name的Json4C_String节点,并添加到object
*/
#define Json4C_AddStringToObject(object,name,s) \
    Json4C_AddItemToObject(object,name,Json4C_CreateString(s))

#define Json4C_SetIntValue(object,value) \
    ((object)?((object)->valueInt=(object)->valueDouble=(value)):(value))

#define Json4C_SetNumberValue(object,value) \
    ((object)?((object)->valueInt=(object)->valueDouble=(value)):(value))

typedef struct Json4C
{
    struct Json4C* next, * prev;
    struct Json4C* child;
    int tpye;

    char* valueString;
    int valueInt;
    double valueDouble;

    //array,object 需要设立节点

    char* string;
}Json4C;

//常用解析函数

/*
功能:
    解析Json数据,将数据填入Json树中
参数:
    value(const char*):char *指针,指向待解析的Json数据
返回值:
    Json树的root节点指针
*/
Json4C* Json4C_Parse(const char* value);

/*
功能:
    从item节点归递遍历,将节点转换为字符串
参数:
    item(Json4C*):Json4C节点
返回值:
    字符串的指针
注意:
    使用后要释放内存
*/
char* Json4C_Print(Json4C* item);

/*
功能:
    从root节点归递遍历,将节点转换为字符串
参数:
    c(Json4C*):Json4C节点
返回值:
    void
*/
void Json4C_Detele(Json4C* c);

/*
功能:
    创建一个Json4C节点,并设置该节点类型为Json4C_Object
参数:
    void
返回值:
    Json4C_Object类型的节点指针
*/
Json4C* Json4C_CreateObject(void);

/*
功能:
    创建一个Json4C节点,并设置该节点类型为Json4C_String
参数:
    str(const char*)
返回值:
    Json4C_String类型的节点指针
*/
Json4C* Json4C_CreateString(const char* str);

/*
功能:
    创建一个Json4C节点,并设置该节点类型为Json4C_Number
参数:
    num(double)
返回值:
    Json4C_Number类型的节点指针
*/
Json4C* Json4C_CreateNumber(double num);

/*
功能:
    创建一个Json4C节点,并设置该节点类型为Json4C_Array
参数:
    void
返回值:
    Json4C_Array类型的节点指针
*/
Json4C* Json4C_CreateArray(void);

/*
功能:
    创建一个Json4C节点,并设置该节点类型为Json4C_Bool
参数:
    boolnumber(int)
返回值:
    Json4C_Bool类型的节点指针
*/
Json4C* Json4C_CreateBool(int b);

/*
功能:
    创建一个Json4C节点,并设置该节点类型为Json4C_True
参数:
    void
返回值:
    Json4C_True类型的节点指针
*/
Json4C* Json4C_CreateTrue(void);

/*
功能:
    创建一个Json4C节点,并设置该节点类型为Json4C_False
参数:
    void
返回值:
    Json4C_False类型的节点指针
*/
Json4C* Json4C_CreateFalse(void);

/*
功能:
    创建一个Json4C节点,并设置该节点类型为Json4C_Null
参数:
    void
返回值:
    Json4C_Null类型的节点指针
*/
Json4C* Json4C_CreateNull(void);

/*
功能:
    将item节点名称设置为string
    如果object没有子节点,就将item设置为object的子节点
    否则将item添加到object->child的链表尾部,成为object子节点的兄弟节点
参数:
    object(Json4C*)
    str(char*)
    item(Json4C*)
返回值:
    void
*/
void Json4C_AddItemToObject(Json4C* object, const char* str, Json4C* item);

/*
功能:
    将item添加到array节点
参数:
    arr(Json4C*)
    item(Json4C*)
返回值:
    void
*/
void Json4C_AddItemToArray(Json4C* arr,Json4C* item);

Json4C* Json4C_ParseWithOptions(const char* value, const char** return_parse_end, int require_null_terminated);

Json4C* Json4C_New_Item();

const char* skip(const char* in);

char* Json4C_GetEndPosition();

const char* parse_value(Json4C* item, const char* value);

const char* parse_string(Json4C* item, const char* str);

const char* parse_number(Json4C* item, const char* num);

const char* parse_array(Json4C* item, const char* value);

const char* parse_object(Json4C* item, const char* value);

//常用打印函数

char* print_value(Json4C* item,int deepth, int fmt);

char* Json4C_strdup(const char* str);

char* print_number(Json4C* item);

char* print_string(Json4C* item, int isName);

char* print_array(Json4C* item, int deepth, int fmt);

char* print_object(Json4C* item, int deepth, int fmt);


Json4C* Json4C_DetachItemFromArray(Json4C* arr, int which);

Json4C* Json4C_DetachItemFromObject(Json4C* object, const char* str);

void Json4C_DeteleItemFromArray(Json4C* arr, int which);

void Json4C_DeteleItemFromObject(Json4C* object, int which);

int Json4C_strcasecmp(const char* str1, const char* str2);


void Json4C_InsertItemInArray(Json4C* arr, int which, Json4C* newItem);

void Json4C_ReplaceItemInArray(Json4C* arr, int which, Json4C* newItem);

void Json4C_ReplaceItemObject(Json4C* object, const char* str, Json4C* newItem);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>

#endif // !_JSON4C_H_