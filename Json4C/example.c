#include "Json4C.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{



	const char* jc = "{\"name\":\"Jack Nimble\",\"format\":{\"type\":\"rect\",\"width\":1920,\"height\":1080,\"interlace\":false,\"frame rate\":24}}";
	Json4C* j = Json4C_CreateObject();
	j = Json4C_Parse(jc);

	char* name = j->child->valueString;

	printf("%s\n", name);









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

	



	return 0;

}