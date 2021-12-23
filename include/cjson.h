#ifndef __CJSON_H_
#define __CJOSN_H_
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <ctype.h>

/***************************************************************************************************
 *
 *  usage: you can just use parse a string to a json object like: cJSON *root = parse(string);
 *         and you can just use exitSafe(); to exit safely because there is a list that is storing
 *         all the node's address, and you can call exitSafe() to release them all;
 *         1. cJSON *root = parse(string);
 *         2. print(...);                   // just parse the json data to console
 *         3. .........                     // do some process to json data in memory
 *         4. char* data = dumpJson();      // you can dump the json from memory
 *         5. char* ep = getErrorPosition();// you can get the error position when parsing
 *
 *  tips:  if you use dumpJson(), you must manually release the string by yourself;
 *         if you dont want to get a format json string, you can change the Macro ISFORMAT to 0.
 *         ISFORMAT  1    <---->    FORMAT JSON OUTPUT
 *         ISFORMAT  0    <---->    WITHOUT FORMAT OUTPUT
 *
 ***************************************************************************************************/
#define ISFORMAT 1

#define ifBadMalloc(ptr) \
    if (!ptr)            \
    { /* bad malloc */   \
        return NULL;     \
    }

#define ifBadParse(ptr)        \
    if (!ptr)                  \
    { /* bad parse position */ \
        return NULL;           \
    }

#define ifNullPtr(ptr)                        \
    if (!ptr)                                 \
    { /* recursion could return a null ptr */ \
        return NULL;                          \
    }

#define voidIfNullPtr(ptr)                    \
    if (!ptr)                                 \
    { /* recursion could return a null ptr */ \
        return;                               \
    }

#if ISFORMAT
#define FORMAT (1)
#else
#define FORMAT (0)
#endif

typedef enum cJSONType
{
    cJSON_FALSE,
    cJSON_TRUE,
    cJSON_NULL,
    cJSON_NUMBER,
    cJSON_STRING,
    cJSON_ARRAY,
    cJSON_OBJECT,
} cJSONType;

typedef enum stringType
{
    keyType,
    dataType,
} stringType;

typedef struct cJSON
{
    /* pointer */
    struct cJSON *next, *prev;
    struct cJSON *child;

    /* data */
    char *key;
    char *stringVal;
    int intVal;
    double doubleVal;

    cJSONType type;
} cJSON;

static cJSON *cjsonList[1024];
static size_t registedNode = 0;
static const char *errorPoistion;

/*******************************************************************************************************
 *  all function here
 *  include parse / print json format content
 *  and create, delete json node, remove, insert, replace node...
 *******************************************************************************************************/
void exitSafe();
const char *getErrorPosition();
/*
 * create and delete node
 */
cJSON *newCjsonNode();
void deleteCjsonNode(cJSON *node);
void removeNodePtrFromList(cJSON *node);

/*
 * parse a string to json format
 */
cJSON *parse(const char *parseString);
cJSON *parseWithOpts(const char *parseString, const char **parseEnd, int justVerity);
const char *skip(const char *position);
const char *parseValue(cJSON *node, const char *value);
const char *parseValueString(cJSON *node, const char *string);
const char *parseValueNumber(cJSON *node, const char *number);
const char *parseValueArray(cJSON *node, const char *array);
const char *parseValueObject(cJSON *node, const char *object);

/*
 * print a json as a string
 */
char *dumpJson(cJSON *root);
void print(cJSON *root);
char *printValue(cJSON *node, int depth, int isFormat);
char *deepCopyConstString(const char *FTNSting);
char *printString(cJSON *node, stringType type);
char *printNumber(cJSON *node);
char *printArray(cJSON *node, int depth, int isFormat);
char *printObject(cJSON *node, int depth, int isFormat);

/*
 * create a new json node
 */
cJSON *createFalseNode(void);
cJSON *createTrueNode(void);
cJSON *createNullNode(void);
cJSON *createStringNode(const char *string);
void setKey(cJSON *nodeDst, const char *key);
cJSON *createNumberNode(double number);
cJSON *createArrayNode(void);
cJSON *createObjectNode(void);

/*
 * add child to array or object
 */
void addNode2Array(cJSON *nodeDst, cJSON *nodeSrc);
void addNode2Object(cJSON *nodeDst, cJSON *nodeSrc, const char *key);

/*
 * string compare, 0 is same, others is different
 */
int keyCmp(const char *s1, const char *s2);

/*
 * if two node same, if it has key judge by key!! otherwise judge the data
 */
int ifSame(const cJSON *c1, const cJSON *c2);
cJSON *judgeIfHasThisNode(cJSON *root, cJSON *judge);

/*
 * find sub node in the root node
 */
cJSON *getSubNodeInArrayByIndex(cJSON *root, int which);
cJSON *getSubNodeInObjectByIndex(cJSON *root, int which);
cJSON *getSubNodeInObjectByKey(cJSON *root, const char *key);

/*
 * remove child from array or object
 */
cJSON *detachNodeFromArrayByIndex(cJSON *node, int which);
cJSON *detachNodeFromObjectByIndex(cJSON *node, int which);
void removeNodeFromArrayByIndex(cJSON *node, int which);
void removeNodeFromObjectByIndex(cJSON *node, int which);
cJSON *detachNodeFromObjectByKey(cJSON *node, const char *key);
void removeNodeFromObjectByKey(cJSON *node, const char *key);

/*
 * insert
 */
void insertNode2Array(cJSON *nodeDst, cJSON *nodeSrc, int which);
void insertNode2Object(cJSON *nodeDst, cJSON *nodeSrc, int which);

/*
 * replace
 */
void replaceNode2ArrayByIndex(cJSON *nodeDst, cJSON *nodeSrc, int which);
void replaceNode2ObjectByIndex(cJSON *nodeDst, cJSON *nodeSrc, int which);
void replaceNode2ObjectByKey(cJSON *nodeDst, cJSON *nodeSrc, const char *key);

#endif