# Json4C
As its name,it's a Json Parser for C programming Language,It is json my prictice.  
  
Thanks to `深剖向`(@510850111),without his video course,I can not make it.  
His Video: https://www.bilibili.com/video/BV1j54y1i7Lq  

# How To Use?

## Parse

There is an json eample,  
```json
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
```

Please include zhis file `Json4C.h`,  
And add the following to your file:  
```c
	const char* jc = "{\"name\":\"Jack Nimble\",\"format\":{\"type\":\"rect\",\"width\":1920,\"height\":1080,\"interlace\":false,\"frame rate\":24}}";
	Json4C* j = Json4C_CreateObject();
	j = Json4C_Parse(jc);

	char* name = j->child->valueString;

	printf("%s\n", name);
```
The Result:  
```
Jack Nimble
```

## Print

You should add some nodes,like zhis:  
```c
Json4C* root = Json4C_CreateObject();
	Json4C* node1 = Json4C_CreateObject();
	Json4C* node2 = Json4C_CreateObject();
	Json4C* node3 = Json4C_CreateObject();

	Json4C_AddFalseToObject(node1, "node1_key1");
	Json4C_AddTrueToObject(node1, "node1_key2");

	Json4C_AddStringToObject(node2, "node2_key1", "node2_value1");
	Json4C_AddStringToObject(node2, "node2_key2", "node2_value2");

	Json4C_AddNumberToObject(node3, "node3_key1", 1000);
	Json4C_AddNumberToObject(node3, "node3_key2", 2000);

	Json4C_AddItemToObject(root, "root_node1", node1);
	Json4C_AddItemToObject(root, "root_node2", node2);

	Json4C_AddItemToObject(node1, "node1_node3", node3);


	char* buf = Json4C_Print(root);
	printf("json:\n%s\n", buf);

	Json4C* parse_node = Json4C_Parse(buf);

	Json4C_Detele(root);
	Json4C_Detele(parse_node);
	free(buf);
```
  
The Result:  
```
json:
{
        "root_node1":   {
                "node1_key1":   false,
                "node1_key2":   true,
                "node1_node3":  {
                        "node3_key1":   1000,
                        "node3_key2":   2000
                }
        },
        "root_node2":   {
                "node2_key1":   "node2_key1",
                "node2_key2":   "node2_key2"
        }
}
```
# Attention
REMEMBER TO FREE PONITERS!!!!!!!!!!!!!
