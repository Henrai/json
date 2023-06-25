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
    Json::JSONObject obj = Json::JSONObject{jsonObj.inner};
    cout << obj << endl;
    auto aa = obj["aa"];
    auto cc = obj["cc"];
    auto d = cc["d"];
    cout << aa << endl;
    cout << cc << endl;
    cout << d  << endl;

    int test = cc["a"].asInt();

    cout << test << endl;
}