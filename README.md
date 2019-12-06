# jsoneek a C++ JSON library

## Overview
jsoneek is simple JSON library for C++.  
 - Only uses standard C++ libraries
 - Parse from text or stream
 - Store data in memory
 - Creatable new node

## Usage
### Integration
jsoneek.h is the single required file. You need to add.
```
#include "jsoneek.h"
```

### Create new JSON

#### Example
```
jsoneek*  pRoot = new jsoneek();
jsoneek*  pInfoNode = new jsoneek("Info");

pInfoNode->AddText("Key1", "ABC");
pInfoNode->AddBool("Key2", true);
pInfoNode->AddNull("Key3");
pInfoNode->AddInt("Key4", -1);
pInfoNode->AddDouble("Key5", 150.2);

pRoot->AddNode(pInfoNode);

jsoneekArray*  pArrayItem = new jsoneekArray("Key6");

pArrayItem->AddText("1");
pArrayItem->AddText("2");
pArrayItem->AddInt(3);

pInfoNode->AddNode(pArrayItem->GetNode());

std::cout << pRoot->GetJsonText(CRT_CRLF, true);

delete pRoot;
```

#### Result
```
{
"Info": {
    "Key1": "ABC",
    "Key2": true,
    "Key3": null,
    "Key4": -1,
    "Key5": 150.200000,
    "Key6": ["1","2",3]
    }
}
```

#### Methods
`new jsoneek()` : Create new node.    
`AddText()` : Create new txet value.  
`AddBool()` : Create new boolean value.  
`AddNull()` : Create new null value.    
`AddInt()` : Create new integer value.  
`AddDouble()` : Create new double value.   
`AddNode()` : Connect to child node.  

### Search & get value
#### Example
```
std::cout << "Key:";
std::cout << pRoot->SearchNode("/Info")->GetKey();
std::cout << "\tType:";
std::cout << pRoot->SearchNode("/Info")->GetValueType();
std::cout << "\n";

std::cout << "Key:";
std::cout << pRoot->SearchNode("/Info/Key1")->GetKey();
std::cout << "\tType:";
std::cout << pRoot->SearchNode("/Info/Key1")->GetValueType();
std::cout << "\tValue:";
std::cout << pRoot->SearchNode("/Info/Key1")->GetTextValue();
std::cout << "\n";

std::cout << "Key:";
std::cout << pRoot->SearchNode("/Info/Key2")->GetKey();
std::cout << "\tType:";
std::cout << pRoot->SearchNode("/Info/Key2")->GetValueType();
std::cout << "\tValue:";
std::cout << pRoot->SearchNode("/Info/Key2")->GetBoolValue();
std::cout << "\n";

std::cout << "Key:";
std::cout << pRoot->SearchNode("/Info/Key3")->GetKey();
std::cout << "\tType:";
std::cout << pRoot->SearchNode("/Info/Key3")->GetValueType();
std::cout << "\tValue:";
std::cout << "\n";

std::cout << "Key:";
std::cout << pRoot->SearchNode("/Info/Key4")->GetKey();
std::cout << "\tType:";
std::cout << pRoot->SearchNode("/Info/Key4")->GetValueType();
std::cout << "\tValue:";
std::cout << pRoot->SearchNode("/Info/Key4")->GetIntValue();
std::cout << "\n";

std::cout << "Key:";
std::cout << pRoot->SearchNode("/Info/Key5")->GetKey();
std::cout << "\tType:";
std::cout << pRoot->SearchNode("/Info/Key5")->GetValueType();
std::cout << "\tValue:";
std::cout << pRoot->SearchNode("/Info/Key5")->GetDoubleValue();
std::cout << "\n";

std::cout << "Key:";
std::cout << pRoot->SearchNode("/Info/Key6")->GetKey();
std::cout << "\tType:";
std::cout << pRoot->SearchNode("/Info/Key6")->GetValueType();
std::cout << "\tValue:";

jsoneekArray* pArrayItem = (jsoneekArray*)pRoot->SearchNode("/Info/Key6");
std::vector<jsoneek*> items;
pArrayItem->GetItemNodes(items);

std::cout << "[0]:";
std::cout << items[0]->GetTextValue();
std::cout << "/";
std::cout << "[1]:";
std::cout << items[1]->GetTextValue();
std::cout << "/";
std::cout << "[2]:";
std::cout << items[2]->GetIntValue();
std::cout << "\n";
```

#### Result
```
Key:Info        Type:5
Key:Key1        Type:4  Value:ABC
Key:Key2        Type:2  Value:1
Key:Key3        Type:1  Value:
Key:Key4        Type:3  Value:-1
Key:Key5        Type:3  Value:150.2
Key:Key6        Type:6  Value:[0]:1/[1]:2/[2]:3
```

#### Methods
`SearchNode()` : Search node from root. Get a pointer if a node is found.  
`GetKey()` : Get key from current node.  
`GetValueType()` : Get type of value from current node.  
`GetTextValue()` : Get text of value from current node.  
`GetBoolValue()` : Get boolean value from current node.  
`GetIntValue()` : Get integer value from current node.  
`GetDoubleValue()` : Get double value from current node.  
`GetItemNodes()` : Get multiple possession nodes from the current node.  

#### Type of value
```
enum VALTYPE {
    VAL_UNKNOWN = 0, // unknown
    VAL_NULL,        // null
    VAL_BOOL,        // boolean
    VAL_NUMBER,      // Number
    VAL_TEXT,        // Text
    VAL_OBJECT,      // Object
    VAL_ARRAY,       // Array
};
```

### Serialize & Deserialize
#### Example
```
jsoneek*     pReadTextNode = jsoneek::AnalizeText("{\"Info\": {\"Key1\": \"ABC\"}}");
std::string  text = pReadTextNode->GetJsonText(CRT_CRLF, true);
delete pReadTextNode;

std::cout << text;
std::cout << "\n";

std::ofstream outfile;
outfile.open("c:\\ProgramData\\file.json", std::ios::out);
outfile.write(text.c_str(), text.length());
outfile.close();

std::ifstream infile;
infile.open("c:\\ProgramData\\file.json", std::ios::in);

jsoneek*  pReadFileNode = jsoneek::AnalizeStream(&infile);
std::cout << pReadFileNode->GetJsonText(CRT_CRLF, true);
delete pReadFileNode;

infile.close();
```

#### Result
```
{
"Info": {
    "Key1": "ABC"
    }
}
{
"Info": {
    "Key1": "ABC"
    }
}
```

#### Methos
`AnalizeText()` : Parse from text.  
`AnalizeStream()` : Parse from stream.  
`GetJsonText()` : Output to text.  

## Version
0.90

## License
Licensed under [MIT License](http://opensource.org/licenses/MIT):

Copyright © 2019 t-ube
