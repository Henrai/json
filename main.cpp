#include <iostream>
#include <string_view>
#include "json.h"


using namespace std;
int main() {
    string_view str = 
    R"JSON({
    'aa': "BB",
    "cc": {
        "a": 10,
        "B": [1,2],
        "c": null,
        "d": true,
        'e': false,
   }})JSON";
    auto [jsonObj, eaten] = Json::JSONObject::parse(str);
    jsonObj.print();
    cout << endl;
}