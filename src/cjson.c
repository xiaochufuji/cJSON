#include "../include/cjson.h"

void exitSafe()
{
    for (size_t i = 0; i < registedNode; ++i)
    {
        if (cjsonList[i])
        { /* not null, release */
            deleteCjsonNode(cjsonList[i]);
        }
    }
}

const char *getErrorPosition()
{
    return errorPoistion;
}

cJSON *newCjsonNode()
{
    cJSON *newNode = (cJSON *)malloc(sizeof(cJSON));
    ifBadMalloc(newNode);
    memset(newNode, 0, sizeof(cJSON));
    cjsonList[registedNode] = newNode;
    registedNode++;
    return newNode;
}

void deleteCjsonNode(cJSON *node)
{
    if (!node)
    {
        perror("free error, invaild pointer");
        return;
    }
    if (node->stringVal)
    {
        free(node->stringVal);
        node->stringVal = NULL;
    }
    if (node->key)
    {
        free(node->key);
        node->key = NULL;
    }
    if (node)
    {
        free(node);
        removeNodePtrFromList(node);
        node = NULL;
    }
}

void removeNodePtrFromList(cJSON *node)
{ /* if the node has manually remove, remove from the list */
    for (size_t i = 0; i < registedNode; ++i)
    {
        if (node == cjsonList[i])
        {
            cjsonList[i] = NULL;
        }
    }
}

cJSON *parse(const char *parseString)
{
    return parseWithOpts(parseString, NULL, 0);
}

cJSON *parseWithOpts(const char *parseString, const char **parseEnd, int justVerity)
{
    cJSON *json = newCjsonNode();
    if (justVerity)
    {
        if (parseValue(json, parseString))
        {
            printf("vaild\n");
        }
        else
        {
            printf("invaild\n");
        }
        deleteCjsonNode(json);
        return NULL;
    }
    /* parse string to a json */
    parseValue(json, parseString);
    if (json && !errorPoistion)
    { /* success to parse a json format */
        return json;
    }
    else
    { /* something error */
        printf("error position: %c\n", *errorPoistion);
        return NULL;
    }
}

const char *skip(const char *position)
{
    if (!position && !(*position))
    {
        return NULL;
    }
    while (*position <= 32)
    {
        ++position;
    }
    // printf("%c", *position);
    return position;
}

const char *parseValue(cJSON *node, const char *value)
{
    if (!value)
    { /* error */
        return NULL;
    }
    if (!strncmp(value, "false", 5))
    { /* false value */
        value += 5;
        node->type = cJSON_FALSE;
    }
    if (!strncmp(value, "true", 4))
    { /* true value */
        value += 4;
        node->type = cJSON_TRUE;
    }
    if (!strncmp(value, "null", 4))
    { /* null value */
        value += 4;
        node->type = cJSON_NULL;
    }
    if (*value == '\"')
    { /* string type */
        return parseValueString(node, value);
    }
    if (*value == '-' || (*value >= '0' && *value <= '9'))
    { /* number type */
        return parseValueNumber(node, value);
    }
    if (*value == '[')
    { /* array type*/
        return parseValueArray(node, value);
    }
    if (*value == '{')
    { /* object type */
        return parseValueObject(node, value);
    }
    return value;
}

const char *parseValueString(cJSON *node, const char *string)
{
    ifNullPtr(string);
    ++string;
    /* deep copy a string */
    size_t copySpace = 0;
    const char *calculateSpace = string;
    while (*calculateSpace != '\"')
    {
        if (*calculateSpace == '\\')
        {
            ++calculateSpace;
            ++copySpace;
        }
        ++calculateSpace;
        ++copySpace;
    }
    char *newString = (char *)malloc(copySpace + 1);
    ifBadMalloc(newString);
    char *tmpString = newString;
    while (*string != '\"')
    {
        if (*string == '\\' && string[1] == '\"')
        {
            *tmpString = *string;
            ++string;
            ++tmpString;
        }
        *tmpString = *string;
        ++string;
        ++tmpString;
    }
    newString[copySpace] = '\0';
    // printf("%s\n", newString);
    // free(newString);
    node->stringVal = newString;

    if (*string == '\"')
    { /* end of string */
        node->type = cJSON_STRING;
        return ++string;
    }
    errorPoistion = string;
    return NULL;
}

const char *parseValueNumber(cJSON *node, const char *number)
{
    ifNullPtr(number);
    int numberSign = 1;   /* number sign */
    int decimalScale = 0; /* if decimal, how scale */
    int scienceSign = 1;  /* the science's index sign */
    int scienceIndex = 0; /* the science' index */
    double result;        /* number calculate result */

    while (*number == '0')
    { /* remove the zero in the head */
        ++number;
    }
    if (*number == '-')
    { /* negative */
        numberSign = -1;
        ++number;
    }
    while (*number >= '0' && *number <= '9')
    { /* interger part */
        result = (result * 10) + (*number - '0');
        ++number;
    }
    if (*number == '.' && number[1] >= '0' && number[1] <= '9')
    { /* decimal part */
        ++number;
        while (*number >= '0' && *number <= '9')
        {
            result = (result * 10) + (*number - '0');
            ++number;
            --decimalScale;
        }
    }
    if (*number == 'E' || *number == 'e')
    { /* science part */
        ++number;
        if (*number == '-')
        { /* science index is - */
            scienceSign = -1;
            ++number;
        }
        while (*number >= '0' && *number <= '9')
        { /* science index */
            scienceIndex = (scienceIndex * 10) + (*number - '0');
            ++number;
        }
    }
    result = numberSign * result * pow(10.0, decimalScale + scienceSign * scienceIndex);
    // printf("%7.4f\n", result);
    node->doubleVal = result;
    node->intVal = (int)result;

    node->type = cJSON_NUMBER;
    return number;
}

const char *parseValueArray(cJSON *node, const char *array)
{
    ifNullPtr(array);
    ++array;
    if (*array == ']')
    { /* special situation empty [] */
        node->type = cJSON_ARRAY;
        node->child = NULL;
        return ++array;
    }
    /* dispose the first child */
    cJSON *firstChild = newCjsonNode();
    ifBadMalloc(firstChild);
    /* link the parent and child */
    node->child = firstChild;
    /* recursion for the first child node */
    array = skip(array);
    array = parseValue(firstChild, array);
    ifBadParse(array);
    /* skip to next sibling node */
    array = skip(array);
    cJSON *tailNode = firstChild;
    while (*array == ',')
    {
        /* skip to next node*/
        cJSON *nextSiBlingNode = newCjsonNode();
        ifBadMalloc(nextSiBlingNode);
        array = skip(++array);
        array = parseValue(nextSiBlingNode, array);
        ifBadParse(array);
        array = skip(array);

        /* link the sibling node, insert to tail */
        tailNode->next = nextSiBlingNode;
        nextSiBlingNode->prev = tailNode;
        tailNode = nextSiBlingNode;
    }
    if (*array == ']')
    { /* end of array */
        node->type = cJSON_ARRAY;
        return ++array;
    }
    errorPoistion = array;
    return NULL;
}

const char *parseValueObject(cJSON *node, const char *object)
{
    ifNullPtr(object);
    ++object;
    /* skip to the object key */
    object = skip(object);
    if (*object == '}')
    { /* special situation for object, empty object {} */
        node->type = cJSON_OBJECT;
        node->child = NULL;
        return ++object;
    }
    cJSON *firstchild = newCjsonNode();
    ifBadMalloc(firstchild);
    /* link the parent and child */
    node->child = firstchild;
    if (*object == '\"')
    { /* hit the key string, this is the first child for this object */
        object = parseValueString(firstchild, object);
        ifBadParse(object);
        /* assign for the key with the foward step's parsed string value */
        firstchild->key = firstchild->stringVal;
        firstchild->stringVal = NULL;
    }
    else
    { /* invaild format: no key */
        errorPoistion = object;
        return NULL;
    }
    /* skip to : */
    object = skip(object);
    if (*object != ':')
    { /* invaild format: no : */
        errorPoistion = object;
        return NULL;
    }
    /* skip to the object data */
    object = skip(++object);
    /* hit the object's data and parse it */
    object = parseValue(firstchild, object);
    ifBadParse(object);
    /* skip to next sibling node */
    object = skip(object);
    cJSON *tailNode = firstchild;
    while (*object == ',')
    {
        cJSON *nextSiBlingNode = newCjsonNode();
        ifBadMalloc(nextSiBlingNode);
        /* skip to next valid key that can parse */
        object = skip(++object);
        if (*object != '\"')
        { /* invaild format: no key*/
            errorPoistion = object;
            return NULL;
        }
        object = parseValueString(nextSiBlingNode, object);
        ifBadParse(object);
        nextSiBlingNode->key = nextSiBlingNode->stringVal;
        nextSiBlingNode->stringVal = NULL;
        object = skip(object);
        if (*object != ':')
        { /* invaild format: no :*/
            errorPoistion = object;
            return NULL;
        }
        /* skip to the object data */
        object = skip(++object);
        /* hit the object's data and parse it */
        object = parseValue(nextSiBlingNode, object);
        ifBadParse(object);
        /* skip to next sibling node */
        object = skip(object);
        /* link the sibling node, insert to tail */
        tailNode->next = nextSiBlingNode;
        nextSiBlingNode->prev = tailNode;
        tailNode = nextSiBlingNode;
    }
    if (*object == '}')
    { /* end of object */
        node->type = cJSON_OBJECT;
        return ++object;
    }
    errorPoistion = object;
    return NULL;
}

/********************************************************************************************************************/

char *dumpJson(cJSON *root)
{
    return printValue(root, 0, FORMAT);
}

void print(cJSON *root)
{
    char *string = printValue(root, 0, FORMAT);
    printf("%s\n", string);
    free(string);
}

char *printValue(cJSON *node, int depth, int isFormat)
{
    char *partialString = NULL; /* the processed node's merged string */
    cJSONType type = node->type;
    switch (type)
    {
    case cJSON_FALSE:
        partialString = deepCopyConstString("false");
        break;
    case cJSON_TRUE:
        partialString = deepCopyConstString("true");
        break;
    case cJSON_NULL:
        partialString = deepCopyConstString("null");
        break;
    case cJSON_STRING:
        partialString = printString(node, dataType);
        break;
    case cJSON_NUMBER:
        partialString = printNumber(node);
        break;
    case cJSON_ARRAY:
        partialString = printArray(node, depth, isFormat);
        break;
    case cJSON_OBJECT:
        partialString = printObject(node, depth, isFormat);
        break;
    default:
        partialString = NULL;
        break;
    }
}

char *deepCopyConstString(const char *FTNSting)
{
    size_t len = strlen(FTNSting) + 1;
    char *partialString = (char *)malloc(len);
    if (!partialString)
    { /* malloc error */
        return NULL;
    }
    memcpy(partialString, FTNSting, len);
    partialString[len] = '\0';
    return partialString;
}

char *printString(cJSON *node, stringType type)
{
    char *forCopyString = NULL;
    char *partialString = NULL;
    if (type == keyType)
    { /* string printed is key */
        forCopyString = node->key;
    }
    else if (type == dataType)
    { /* string printed is data */
        forCopyString = node->stringVal;
    }
    size_t len = strlen(forCopyString) + 1 + 2; /* 2 for 2 \" at first and end  */
    partialString = (char *)malloc(len);
    if (!partialString)
    {
        return NULL;
    }
    /* construct the string to return  */
    char *tmpPtr = partialString;
    *tmpPtr++ = '\"';
    memcpy(tmpPtr, forCopyString, len - 2);
    tmpPtr += (len - 3);
    *tmpPtr++ = '\"';
    *tmpPtr = '\0';
    // printf("%s\n", partialString);
    return partialString;
}

char *printNumber(cJSON *node)
{
    double doublval = node->doubleVal;
    int intVal = node->intVal;
    char *partialString = NULL;
    if (doublval == 0.0)
    { /* zero */
        partialString = (char *)malloc(2);
        if (!partialString)
        {
            return NULL;
        }
        memcpy(partialString, "0\0", 2);
        return partialString;
    }
    else if (fabs((double)intVal - doublval) <= DBL_EPSILON && intVal <= INT_MAX && intVal >= INT_MIN)
    { /* interger */
        partialString = (char *)malloc(21);
        if (!partialString)
        {
            return NULL;
        }
        sprintf(partialString, "%d", intVal);
    }
    else
    { /* float */
        partialString = (char *)malloc(64);
        if (!partialString)
        {
            return NULL;
        }
        if (fabs(floor(doublval) - doublval) < DBL_EPSILON && fabs(doublval) < 1.e60)
        {
            sprintf(partialString, "%.0f", doublval);
        }
        else if (fabs(doublval) < 1.0e-6 || fabs(doublval) > 1.0e9)
        { /* science */
            sprintf(partialString, "%e", doublval);
        }
        else
        {
            sprintf(partialString, "%f", doublval);
        }
    }
    // printf("%s\n", partialString);
    return partialString;
}

char *printArray(cJSON *node, int depth, int isFormat)
{
    char *partialString = NULL;
    size_t len = 3; /* space for '[',']'and '\0' */
    size_t chilidNums = 0;
    cJSON *firstChild = node->child;
    while (firstChild)
    {
        ++chilidNums;
        firstChild = firstChild->next;
    }
    /* is a empty array [] */
    if (chilidNums == 0)
    {
        partialString = (char *)malloc(len);
        memcpy(partialString, "[]\0", len);
        return partialString;
    }
    /*
     * a vector to store all the partialString pointer from chilid in the array, calculate the space
     */
    char **partialStringVector = (char **)malloc(chilidNums * sizeof(char *));
    if (!partialStringVector)
    { /* malloc error */
        return NULL;
    }
    firstChild = node->child;
    for (size_t i = 0; i < chilidNums; ++i)
    { /* for each the array and calculate how big should malloc */
        partialStringVector[i] = printValue(firstChild, depth + 1, isFormat);
        len += strlen(partialStringVector[i]); /* total length add the partial langth */
        firstChild = firstChild->next;
    }
    len += (chilidNums - 1); /* total length add the commas's numbers */
    if (isFormat)
    {                                            /* is format, add the line breadk(\n) and table(\t) numbers */
        len += (chilidNums + 1);                 /* \n numbers */
        len += chilidNums * (depth + 1) + depth; /* \t numbers */
    }
    /*
     *  malloc length
     */
    partialString = (char *)malloc(len);
    if (!partialString)
    { /* malloc error */
        return NULL;
    }
    /*
     *  construct the array string to return
     */
    char *tmpPtr = partialString;
    *tmpPtr++ = '[';
    if (isFormat)
    {
        *tmpPtr++ = '\n';
    }
    for (size_t i = 0; i < chilidNums; ++i)
    { /* for each the array and copy from the partialvector */
        size_t copyLen = strlen(partialStringVector[i]);
        if (isFormat)
        {
            for (int i = 0; i <= depth; ++i)
            {
                *tmpPtr++ = '\t';
            }
        }
        memcpy(tmpPtr, partialStringVector[i], copyLen);
        free(partialStringVector[i]); /* free the chilid partial string */
        partialStringVector[i] = NULL;
        tmpPtr += copyLen; /* move to next position to create a comma*/
        if (i < chilidNums - 1)
        {
            *tmpPtr++ = ',';
        }
        if (isFormat)
        {
            *tmpPtr++ = '\n';
        }
    }
    if (isFormat)
    { /* end of array */
        for (int i = 0; i < depth; ++i)
        {
            *tmpPtr++ = '\t';
        }
    }
    *tmpPtr++ = ']';
    *tmpPtr = '\0';
    if (len - 1 != strlen(partialString))
    {
        perror("heap size malloc wrong!");
    }
    // printf("strlen: %d\nmalloc len: %d\n", strlen(partialString), len);
    // printf("%s\n", partialString);
    return partialString;
}

char *printObject(cJSON *node, int depth, int isFormat)
{
    char *partialString = NULL;
    size_t len = 3; /* space for '{','}'and '\0' */
    size_t chilidNums = 0;
    cJSON *firstChild = node->child;
    while (firstChild)
    {
        ++chilidNums;
        firstChild = firstChild->next;
    }
    /* is a empty object {} */
    if (chilidNums == 0)
    {
        partialString = (char *)malloc(len);
        memcpy(partialString, "{}\0", len);
        return partialString;
    }
    /*
     * a vector to store all the partialString pointer from chilid in the object, calculate the space
     * store object's key, key!!!, it must be a string
     */
    char **partialStringVector = (char **)malloc(chilidNums * sizeof(char *));
    if (!partialStringVector)
    { /* malloc error */
        return NULL;
    }
    firstChild = node->child;
    for (size_t i = 0; i < chilidNums; ++i)
    { /* for each the object's key and calculate how big should malloc */
        partialStringVector[i] = printString(firstChild, keyType);
        len += strlen(partialStringVector[i]); /* total length add the partial langth */
        firstChild = firstChild->next;
    }
    /*
     * a vector to store all the partialString pointer from chilid in the object, calculate the space
     * store object's data, data!!!
     */
    char **partialStringVectorTwo = (char **)malloc(chilidNums * sizeof(char *));
    if (!partialStringVectorTwo)
    { /* malloc error */
        return NULL;
    }
    firstChild = node->child;
    for (size_t i = 0; i < chilidNums; ++i)
    { /* for each the object's key and calculate how big should malloc */
        partialStringVectorTwo[i] = printValue(firstChild, depth + 1, isFormat);
        len += strlen(partialStringVectorTwo[i]); /* total length add the partial langth */
        firstChild = firstChild->next;
    }
    len += (chilidNums - 1); /* total length add the commas's(,) numbers */
    len += chilidNums;       /* total length add the colon's(:) numbers */
    if (isFormat)
    {                                            /* is format, add the line breadk(\n) and table(\t) numbers */
        len += (chilidNums + 1);                 /* \n numbers */
        len += chilidNums * (depth + 1) + depth; /* \t numbers */
        len += chilidNums;                       /* space after colon's(:) */
    }
    /*
     *  malloc length
     */
    partialString = (char *)malloc(len);
    if (!partialString)
    { /* malloc error */
        return NULL;
    }
    /*
     *  construct the array string to return
     */
    char *tmpPtr = partialString;
    *tmpPtr++ = '{';
    if (isFormat)
    {
        *tmpPtr++ = '\n';
    }
    for (size_t i = 0; i < chilidNums; ++i)
    { /* for each the array and copy from the partialvector */
        /*
         *  key merge!!!
         */
        size_t copyLen = strlen(partialStringVector[i]);
        if (isFormat)
        {
            for (int i = 0; i <= depth; ++i)
            {
                *tmpPtr++ = '\t';
            }
        }
        memcpy(tmpPtr, partialStringVector[i], copyLen);
        free(partialStringVector[i]); /* free the chilid partial string */
        partialStringVector[i] = NULL;
        tmpPtr += copyLen; /* move to next position to create a comma*/
        *tmpPtr++ = ':';
        if (isFormat)
        {
            *tmpPtr++ = ' ';
        }
        /*
         *  data merge!!!
         */
        copyLen = strlen(partialStringVectorTwo[i]);
        memcpy(tmpPtr, partialStringVectorTwo[i], copyLen);
        free(partialStringVectorTwo[i]); /* free the chilid partial string */
        partialStringVectorTwo[i] = NULL;
        tmpPtr += copyLen; /* move to next position to create a comma*/
        if (i < chilidNums - 1)
        {
            *tmpPtr++ = ',';
        }
        if (isFormat)
        {
            *tmpPtr++ = '\n';
        }
    }
    if (isFormat)
    { /* end of object */
        for (int i = 0; i < depth; ++i)
        {
            *tmpPtr++ = '\t';
        }
    }
    *tmpPtr++ = '}';
    *tmpPtr = '\0';
    if (len - 1 != strlen(partialString))
    {
        perror("heap size malloc wrong!");
    }
    // printf("strlen: %d\nmalloc len: %d\n", strlen(partialString), len);
    // printf("%s\n", partialString);
    return partialString;
}

/*********************************************************************************************************************/

cJSON *createFalseNode(void)
{
    cJSON *newNode = newCjsonNode();
    ifBadMalloc(newNode);
    newNode->type = cJSON_FALSE;
    return newNode;
}

cJSON *createTrueNode(void)
{
    cJSON *newNode = newCjsonNode();
    ifBadMalloc(newNode);
    newNode->type = cJSON_TRUE;
    return newNode;
}

cJSON *createNullNode(void)
{
    cJSON *newNode = newCjsonNode();
    ifBadMalloc(newNode);
    newNode->type = cJSON_NULL;
    return newNode;
}

cJSON *createStringNode(const char *string)
{
    cJSON *newNode = newCjsonNode();
    ifBadMalloc(newNode);
    newNode->type = cJSON_STRING;
    newNode->stringVal = deepCopyConstString(string);
    return newNode;
}

void setKey(cJSON *nodeDst, const char *key)
{
    if (nodeDst->key)
    { /* if change */
        free(nodeDst->key);
        nodeDst->key = NULL;
    }
    nodeDst->key = deepCopyConstString(key);
}

cJSON *createNumberNode(double number)
{
    cJSON *newNode = newCjsonNode();
    ifBadMalloc(newNode);
    newNode->type = cJSON_NUMBER;
    newNode->doubleVal = number;
    newNode->intVal = (int)number;
    return newNode;
}

cJSON *createArrayNode(void)
{
    cJSON *newNode = newCjsonNode();
    ifBadMalloc(newNode);
    newNode->type = cJSON_ARRAY;
    return newNode;
}

cJSON *createObjectNode(void)
{
    cJSON *newNode = newCjsonNode();
    ifBadMalloc(newNode);
    newNode->type = cJSON_OBJECT;
    return newNode;
}

/************************************************************************************************************************/

void addNode2Array(cJSON *nodeDst, cJSON *nodeSrc)
{
    if (!nodeDst || nodeDst->type != cJSON_ARRAY)
    {
        perror("the dst node is not a array");
        return;
    }
    cJSON *locatedChild = nodeDst->child;
    if (!locatedChild)
    { /* first child is empty */
        nodeDst->child = locatedChild;
        return;
    }
    while (locatedChild->next)
    { /* index to the last child */
        locatedChild = locatedChild->next;
    }
    locatedChild->next = nodeSrc;
    nodeSrc->prev = locatedChild;
}

void addNode2Object(cJSON *nodeDst, cJSON *nodeSrc, const char *key)
{
    if (!nodeDst || nodeDst->type != cJSON_OBJECT)
    {
        return;
    }
    cJSON *locatedChild = nodeDst->child;
    if (!locatedChild)
    { /* first child is empty */
        nodeDst->child = nodeSrc;
        setKey(nodeSrc, key);
        return;
    }
    while (locatedChild->next)
    { /* index to the last child */
        locatedChild = locatedChild->next;
    }
    setKey(nodeSrc, key);
    locatedChild->next = nodeSrc;
    nodeSrc->prev = locatedChild;
}

/**********************************************************************************************************************/

int keyCmp(const char *s1, const char *s2)
{
    if (!s1 || !s1)
    { /* invaild pointer */
        return 1;
    }
#if ISCASESENSITIVE
    while ((*s1) == (*s2))
#else
    while (tolower(*s1) == tolower(*s2))
#endif
    {
        ++s1;
        ++s2;
        if (*s1 == '\0' && *s2 == '\0')
        {
            return 0;
        }
    }
    return 1;
}

/**********************************************************************************************************************/

int ifSame(const cJSON *c1, const cJSON *c2)
{
    if (!c1->key && !c2->key)
    {
        if (c1->type == cJSON_NULL && c2->type == cJSON_NULL)
            return 0;
        else if (c1->type == cJSON_FALSE && c2->type == cJSON_FALSE)
            return 0;
        else if (c1->type == cJSON_TRUE && c2->type == cJSON_TRUE)
            return 0;
        else if (c1->type == cJSON_NUMBER && c2->type == cJSON_NUMBER)
        {
            return c1->doubleVal != c2->doubleVal;
        }
        else if (c1->type == cJSON_STRING && c2->type == cJSON_STRING)
        {
            return strcmp(c1->stringVal, c2->stringVal);
        }
        return 0; /* arbitrarily add array or object */
    }
    else if (c1->key && !c2->key)
    {
        return 1;
    }
    else if (!c1->key && c2->key)
    {
        return 1;
    }
    else if (c1->key && c2->key)
    {
        return keyCmp(c1->key, c2->key);
    }
}

cJSON *judgeIfHasThisNode(cJSON *root, cJSON *judge)
{
    cJSON *iterator = root->child;
    while (iterator)
    {
        if (!ifSame(iterator, judge))
        {
            return iterator;
        }
        iterator = iterator->next;
    }
    return NULL;
}

/**********************************************************************************************************************/

cJSON *getSubNodeInArrayByIndex(cJSON *root, int which)
{
    if (!root || which < 0 || (root->type != cJSON_ARRAY && root->type != cJSON_OBJECT))
    {
        perror("wrong node you choose");
        return NULL;
    }
    cJSON *locatedChild = root->child;
    if (!locatedChild)
    {
        perror("it's a empty container");
        return NULL;
    }
    if (which == 0)
    { /* first child */
        return locatedChild;
    }
    else
    { /* other child */
        while (locatedChild && which > 0)
        { /* from 1 */
            locatedChild = locatedChild->next;
            --which;
        }
        if (!locatedChild)
        {
            perror("out of range");
            return NULL;
        }
        return locatedChild;
    }
}

cJSON *getSubNodeInObjectByIndex(cJSON *root, int which)
{
    return getSubNodeInArrayByIndex(root, which);
}

cJSON *getSubNodeInObjectByKey(cJSON *root, const char *key)
{
    if (!root || !key || root->type != cJSON_OBJECT)
    { /* invaild key or node */
        perror("wrong node you choose");
        return NULL;
    }
    cJSON *locatedChild = root->child;
    if (!root->child)
    {
        perror("it's a empty container");
        return NULL;
    }
    if (!keyCmp(locatedChild->key, key))
    { /* the first child */
        return locatedChild;
    }
    else
    { /* the sibling */
        while (locatedChild && keyCmp(locatedChild->key, key))
        {
            locatedChild = locatedChild->next;
        }
        if (!locatedChild)
        { /* child run to the end */
            perror("there's no a node has such a key");
            return NULL;
        }
        return locatedChild;
    }
}

/**********************************************************************************************************************/

cJSON *detachNodeFromArrayByIndex(cJSON *node, int which)
{
    cJSON *locatedChild = getSubNodeInArrayByIndex(node, which);
    ifNullPtr(locatedChild);
    if (which == 0)
    { /* first child */
        node->child = locatedChild->next;
    }
    else
    { /* other child(sibling) */
        if (locatedChild->prev)
        {
            locatedChild->prev->next = locatedChild->next;
            locatedChild->prev = NULL;
        }
    }
    if (locatedChild->next)
    { /* the first node has no prve node, just next node */
        locatedChild->next->prev = locatedChild->prev;
        locatedChild->next = NULL;
    }
    return locatedChild;
}

cJSON *detachNodeFromObjectByIndex(cJSON *node, int which)
{
    return detachNodeFromArrayByIndex(node, which);
}

void removeNodeFromArrayByIndex(cJSON *node, int which)
{
    deleteCjsonNode(detachNodeFromArrayByIndex(node, which));
}

void removeNodeFromObjectByIndex(cJSON *node, int which)
{
    deleteCjsonNode(detachNodeFromArrayByIndex(node, which));
}

cJSON *detachNodeFromObjectByKey(cJSON *node, const char *key)
{
    cJSON *locatedChild = getSubNodeInObjectByKey(node, key);
    ifNullPtr(locatedChild);
    if (!keyCmp(node->child->key, key))
    { /* the first child */
        node->child = node->child->next;
    }
    else
    { /* other child(sibling) */
        if (locatedChild->prev)
        {
            locatedChild->prev->next = locatedChild->next;
            locatedChild->prev = NULL;
        }
    }
    if (locatedChild->next)
    { /* the first node has no prve node, just next node */
        locatedChild->next->prev = locatedChild->prev;
        locatedChild->next = NULL;
    }
    return locatedChild;
}

void removeNodeFromObjectByKey(cJSON *node, const char *key)
{
    deleteCjsonNode(detachNodeFromObjectByKey(node, key));
}

/**********************************************************************************************************************/

void insertNode2Array(cJSON *nodeDst, cJSON *nodeSrc, int which)
{
    if (!nodeDst || which < 0 || (nodeDst->type != cJSON_ARRAY && nodeDst->type != cJSON_OBJECT))
    {
        perror("wrong dst node or src node, it could not be a array or a object");
        return;
    }
    if (nodeDst->type == cJSON_OBJECT && !nodeSrc->key)
    {                                           /* if insert to a object and nodesrc has no key */
        nodeSrc->key = deepCopyConstString(""); /* add a null key */
    }
    /*
     * judge if has the same node
     */
    cJSON *judge = judgeIfHasThisNode(nodeDst, nodeSrc);
    if (judge)
    { /* has the same node */
        perror("has the same node in dst");
        return;
    }
    /*
     * actually insert to parent node
     */
    cJSON *locatedChild = nodeDst->child;
    if (which == 0)
    { /* first child */
        nodeDst->child = nodeSrc;
        nodeSrc->next = locatedChild;
        if (locatedChild)
        { /* it could be a empty node */
            locatedChild->prev = nodeSrc;
        }
    }
    else
    { /* other child */
        while (locatedChild && --which > 0)
        { /* from 0 */
            locatedChild = locatedChild->next;
        }
        if (which != 0 && !locatedChild)
        {
            perror("out of range");
            return;
        }
        nodeSrc->prev = locatedChild;
        nodeSrc->next = locatedChild->next;
        locatedChild->next = nodeSrc;
        if (nodeSrc->next)
        { /* if has next sibling */
            nodeSrc->next->prev = nodeSrc;
        }
    }
}

void insertNode2Object(cJSON *nodeDst, cJSON *nodeSrc, int which)
{
    insertNode2Array(nodeDst, nodeSrc, which);
}

/***********************************************************************************************************************/

void replaceNode2ArrayByIndex(cJSON *nodeDst, cJSON *nodeSrc, int which)
{
    cJSON *locatedChild;
    if (nodeDst->type != cJSON_ARRAY && nodeDst->type != cJSON_OBJECT)
    { /* wrong type */
        perror("wrong type of dst you choose, it should be a array or a object");
        return;
    }
    // if (nodeDst->type == cJSON_OBJECT && !nodeSrc->key)
    // {                                           /* if insert to a object and nodesrc has no key */
    //     nodeSrc->key = deepCopyConstString(""); /* add a null key */
    // }
    /*
     * judge if has the same node
     */
    cJSON *judge = judgeIfHasThisNode(nodeDst, nodeSrc);
    if (judge)
    { /* has the same node, change it to new */
        locatedChild = judge;
        if (!locatedChild->prev)
        { /* is the first child */
            nodeDst->child = nodeSrc;
        }
        nodeSrc->next = locatedChild->next;
        nodeSrc->prev = locatedChild->prev;
        if (locatedChild->prev)
        {
            locatedChild->prev->next = nodeSrc;
        }
        if (locatedChild->next)
        {
            locatedChild->next->prev = nodeSrc;
        }
    }
    else
    { /* otherwise do the normal thing */
        locatedChild = getSubNodeInArrayByIndex(nodeDst, which);
        voidIfNullPtr(locatedChild);
        /*
         * actually replace work
         */
        if (which == 0)
        { /* first child */
            nodeSrc->next = locatedChild->next;
            nodeDst->child = nodeSrc;
            if (locatedChild->next)
            { /* the next node could be a empty node */
                locatedChild->next->prev = nodeSrc;
            }
        }
        else
        { /* other child */
            nodeSrc->prev = locatedChild->prev;
            nodeSrc->next = locatedChild->next;
            if (locatedChild->next)
            {
                nodeSrc->next->prev = nodeSrc;
            }
            if (locatedChild->prev)
            {
                nodeSrc->prev->next = nodeSrc;
            }
        }
    }
    deleteCjsonNode(locatedChild);
}

void replaceNode2ObjectByIndex(cJSON *nodeDst, cJSON *nodeSrc, int which)
{
    replaceNode2ArrayByIndex(nodeDst, nodeSrc, which);
}

void replaceNode2ObjectByKey(cJSON *nodeDst, cJSON *nodeSrc, const char *key)
{
    cJSON *locatedChild;
    if (nodeDst->type != cJSON_OBJECT)
    { /* wrong type */
        perror("wrong type of dst you choose, it should be a object");
        return;
    }
    if (nodeDst->type == cJSON_OBJECT && !nodeSrc->key)
    {                                           /* if insert to a object and nodesrc has no key */
        nodeSrc->key = deepCopyConstString(""); /* add a null key */
    }
    /*
     * judge if has the same node
     */
    cJSON *judge = judgeIfHasThisNode(nodeDst, nodeSrc);
    if (judge)
    { /* has the same node */
        locatedChild = judge;
        if (!locatedChild->prev)
        { /* is the first child */
            nodeDst->child = nodeSrc;
        }
        nodeSrc->next = locatedChild->next;
        nodeSrc->prev = locatedChild->prev;
        if (locatedChild->prev)
        {
            locatedChild->prev->next = nodeSrc;
        }
        if (locatedChild->next)
        {
            locatedChild->next->prev = nodeSrc;
        }
    }
    else
    { /* otherwise do the normal thing */
        locatedChild = getSubNodeInObjectByKey(nodeDst, key);
        voidIfNullPtr(locatedChild);
        /*
         * actually replace work
         */
        if (!keyCmp(nodeDst->child->key, key))
        { /* the first child */
            nodeSrc->next = locatedChild->next;
            nodeDst->child = nodeSrc;
            if (locatedChild->next)
            { /* the next node could be a empty node */
                locatedChild->next->prev = nodeSrc;
            }
        }
        else
        { /* the sibling */
            nodeSrc->prev = locatedChild->prev;
            nodeSrc->next = locatedChild->next;
            if (locatedChild->prev)
            {
                locatedChild->prev->next = nodeSrc;
            }
            if (nodeSrc->next)
            {
                locatedChild->next->prev = nodeSrc;
            }
        }
    }
    deleteCjsonNode(locatedChild);
}
