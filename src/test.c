#include "../include/cjson.h"

int main()
{
    char buffer[256];
    memset(buffer, 0, 256);
    FILE *fp = fopen("../bin/test.json", "r");
    fread(buffer, 256, 1, fp);
    cJSON *json = parse(buffer);
    if (json)
    {
        /**************************************************************************************/
        print(json);
        /**************************************************************************************/
        cJSON *a = createStringNode("a string");
        addNode2Object(json, a, "a key");
        print(json);
        /**************************************************************************************/
        cJSON *b = createNumberNode(234.567);
        if (!json->child->next) /* ensure json->child->next exits */
            goto HERE;
        addNode2Array(json->child->next, b);
        print(json);
        /**************************************************************************************/
        if (!json->child->next) /* ensure json->child->next exits */
            goto HERE;
        cJSON *c = detachNodeFromArrayByIndex(json->child->next, 1);
        if (!c) /* ensure c exits */
            goto HERE;
        print(c);
        print(json);
        /**************************************************************************************/
        removeNodeFromObjectByIndex(json, 0);
        print(json);
        /**************************************************************************************/
        cJSON *d = detachNodeFromObjectByKey(json, "A key");
        if (!d) /* ensure d exits */
            goto HERE;
        print(d);
        print(json);
        /**************************************************************************************/
        cJSON *e = createArrayNode();
        cJSON *f = createStringNode("insert to array");
        cJSON *h = createNumberNode(8888);
        insertNode2Array(e, f, 0);
        addNode2Array(e, h);
        setKey(e, "new array node");
        insertNode2Object(json, e, 1);
        print(e);
        print(json);
        /**************************************************************************************/
        cJSON *i = createStringNode("replace the array node 8888");
        if (!json->child->next) /* ensure json->child->next exits */
            goto HERE;
        replaceNode2ArrayByIndex(json->child->next, i, 1);
        print(json);
        /**************************************************************************************/
        if (json->child->child->next->next) /* ensure json->child->child->next->next exits */
            goto HERE;
        replaceNode2ObjectByKey(json->child->child->next->next, d, "array object");
        print(json);
    }
    printf("error position: %c\n", *getErrorPosition());
HERE:
    exitSafe();
    fclose(fp);
    return 0;
}