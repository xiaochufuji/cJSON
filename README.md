## *what I use*

```bash
MINGW64 8.1.0
vscode
git bash 4.4.23(1)
cmake 3.20.6

so the command i use when cmake in build/ is
`cmake -G "MinGW Makefiles" ..`
the command i use when makefile in build/ is 
`mingw32-make`
```





## ***the following content is what test file display***

### display 1st

```json
{
        "test": 123,
        "key value": [
                "01string",
                "02string",
                123.456000,
                {
                        "array object": "nice"
                },
                [
                        false,
                        null,
                        true
                ]
        ]
}
```



### display 2nd: 

`addNode2Object(json, a, "a key");`

```json
{
        "test": 123,
        "key value": [
                "01string",
                "02string",
                123.456000,
                {
                        "array object": "nice"
                },
                [
                        false,
                        null,
                        true
                ]
        ],
        "a key": "a string"
}
```



### display 3rd:

`addNode2Array(json->child->next, b);`

```json
{
        "test": 123,
        "key value": [
                "01string",
                "02string",
                123.456000,
                {
                        "array object": "nice"
                },
                [
                        false,
                        null,
                        true
                ],
                234.567000
        ],
        "a key": "a string"
}
```

 

### display 4th:

`cJSON *c = detachNodeFromArrayByIndex(json->child->next, 1);`

```json
"02string"
```

```json
{
        "test": 123,
        "key value": [
                "01string",
                123.456000,
                {
                        "array object": "nice"
                },
                [
                        false,
                        null,
                        true
                ],
                234.567000
        ],
        "a key": "a string"
}
```



### display 5th:

`removeNodeFromObjectByIndex(json, 0);`

```json
{
        "key value": [
                "01string",
                123.456000,
                {
                        "array object": "nice"
                },
                [
                        false,
                        null,
                        true
                ],
                234.567000
        ],
        "a key": "a string"
}
```



### display 6th:

`cJSON *d = detachNodeFromObjectByKey(json, "a key");`

```json
"a string"
```

```json
{
        "key value": [
                "01string",
                123.456000,
                {
                        "array object": "nice"
                },
                [
                        false,
                        null,
                        true
                ],
                234.567000
        ]
}
```



### display 7th:

```c
cJSON *e = createArrayNode();
cJSON *f = createStringNode("insert to array");
cJSON *h = createNumberNode(8888);
insertNode2Array(e, f, 0);
addNode2Array(e, h);
setKey(e, "new array node");
insertNode2Object(json, e, 1);
```

e block

```json
[
        "insert to array",
        8888
]
```

```json
{
        "key value": [
                "01string",
                123.456000,
                {
                        "array object": "nice"
                },
                [
                        false,
                        null,
                        true
                ],
                234.567000
        ],
        "new array node": [
                "insert to array",
                8888
        ]
}
```



### display 8th:

```c
cJSON *i = createStringNode("replace the array node 8888");
replaceNode2ArrayByIndex(json->child->next, i, 1);
```

```json
{
        "key value": [
                "01string",
                123.456000,
                {
                        "array object": "nice"
                },
                [
                        false,
                        null,
                        true
                ],
                234.567000
        ],
        "new array node": [
                "insert to array",
                "replace the array node 8888"
        ]
}
```



### display 9th:

`replaceNode2ObjectByKey(json->child->child->next->next,d,"array object");`

use a node detached(cJSON pointer d) in the past, replace the string(key is "array object")

```json
{
        "key value": [
                "01string",
                123.456000,
                {
                        "a key": "a string"
                },
                [
                        false,
                        null,
                        true
                ],
                234.567000
        ],
        "new array node": [
                "insert to array",
                "replace the array node 8888"
        ]
}
```

