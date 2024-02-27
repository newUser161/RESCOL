#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <iostream>
#include <string>

using namespace rapidjson;
using namespace std;

class MyHandler : public BaseReaderHandler<UTF8<>, MyHandler> {
private:
    bool foundOrigin = false;
    bool foundDestination = false;
    string currentNode;
    string targetOrigin = "2";
    string targetDestination = "14";

public:
    bool StartObject() {
        return true;
    }

    bool Key(const char* str, SizeType length, bool copy) {
        currentNode = string(str, length);
        if (currentNode == targetOrigin) {
            foundOrigin = true;
        }
        return true;
    }

    bool EndObject(SizeType memberCount) {
        if (foundOrigin && foundDestination) {
            // Finaliza el análisis cuando se encuentra la ruta deseada
            return false;
        }
        foundOrigin = false;
        return true;
    }

    bool StartArray() {
        if (foundOrigin && currentNode == targetDestination) {
            foundDestination = true;
        }
        return true;
    }

    bool Int(int i) {
        if (foundOrigin && foundDestination) {
            cout << i << " ";
        }
        return true;
    }

    bool EndArray(SizeType elementCount) {
        if (foundOrigin && foundDestination) {
            cout << endl;
            // Finaliza el análisis después de imprimir la ruta
            return false;
        }
        return true;
    }
};
