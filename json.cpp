#include "json.h"
#include <algorithm>
#include <cstddef>
#include <utility>
#include <variant>
#include <charconv>
#include <regex>

namespace Json {

    bool JSONObject::canEatToken(std::string_view json, std::string_view token) {
        int eaten = 0;
        auto [_, token_match] = std::mismatch(json.begin(), json.end(), token.begin(), token.end());
        if (token_match == token.end()) {
            return true;
        }
        return false;
    }

    std::pair<JSONObject, size_t> JSONObject::parse(std::string_view json) {
        if(json.empty())  {
            return {JSONObject{std::monostate{}} , 0};
        } else if (size_t offset= json.find_first_not_of(" \n\t\r\0\f\v"); offset != 0 && offset != std::string::npos) {
            auto [obj, eaten] = parse(json.substr(offset));
            return {obj, eaten+offset};
        } else if ('0' <= json[0] && json[0] <= '9' || json[0] == '+' || json[0] == '-') {
            std::regex num_re{"[-+]?(?:\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][-+]?\\d+)?"};
            std::cmatch match;
            if (std::regex_search(json.data(), json.data() + json.size(), match, num_re)) {
                std::string str = match.str();
                if(auto num = tryParseNum<int>(str)) {
                    return {JSONObject{*num}, str.size()};
                } 
                else if (auto num = tryParseNum<double>(str)) {
                    return {JSONObject{*num}, str.size()};
                }
            }
        } else if (json[0] == '"' || json[0] == '\'') {
            char quote = json[0];
            std::string str;
            enum {
                RAW,
                ESCAPE,
            } phase = RAW;
            size_t i = 1;
            while(i < json.size()) {
                char ch = json[i++];
                if(phase == RAW) {
                    if (ch == '\\') {
                        phase = ESCAPE;
                    } else if(ch == quote) {
                        break;
                    } else {
                        str += ch;
                    }
                } else if (phase == ESCAPE) {
                    str += unescape_char(ch);
                    phase = RAW;
                }
            } 
            return {JSONObject{std::move(str)}, i};
        } else if (json[0] == '[') {
            JSONList res;
            size_t i =1;
            while(i < json.size()) {
                if (json[i] == ']') {
                    i++;
                    break;
                }
                auto [obj, eaten] = parse(json.substr(i));
                if (eaten == 0) {
                    break;
                }
                res.push_back(std::move(obj));
                i+= eaten;
                if(json[i] == ',') {
                    i++;
                }
            } 
            return {JSONObject{std::move(res)}, i};
        } else if (json[0] == '{') {
            JSONMap res;
            size_t i = 1;
            while (i < json.size()) {
                //std::cout<<json.substr(i) << std::endl << "-------" << std::endl;
                if (json[i] == '}') {
                    i++;
                    break;
                }
                auto [key_obj, key_eaten] = parse(json.substr(i));
                if (key_eaten == 0) {
                    i = 0; 
                    break;
                }
                i += key_eaten;
                if (!std::holds_alternative<std::string>(key_obj.inner)) {
                    i = 0;
                    break;
                }

                std::string key = std::move(std::get<std::string>(key_obj.inner)); 
                
                if (json[i] == ':') {
                    i++;
                }

                auto [value_obj, val_eaten] = parse(json.substr(i));
                if (val_eaten == 0) {
                    i = 0;
                    break;
                }
                i += val_eaten;
                res.insert_or_assign(std::move(key), std::move(value_obj));
                if(json[i] == ',') {
                    i++;
                }
            }
            return {JSONObject{std::move(res)}, i};
        } else if (canEatToken(json, "null")) {
            return {JSONObject{std::monostate{}}, 4};
        } else if (canEatToken(json, "true")) {
            return {JSONObject{true}, 4};
        } else if(canEatToken(json, "false")) {
            return {JSONObject{false}, 5};
        }

        JSONObject jsonobj= JSONObject{std::monostate{}}; 
        return {jsonobj, 0};
    }

    void JSONObject::print() const {
            std::visit(
                overloaded{
                [this] (const JSONList& list){
                    std::cout << "[";
                    for (int i = 0; i < list.size(); i++) {
                        list[i].print();
                        if(i != list.size() -1) {
                            std::cout << ", ";
                        }
                    }
                    std::cout << "]";
                },
                [this] (const JSONMap& map) {
                    std::cout << "{";
                    for (auto it = map.begin(); it != map.end(); it++) {
                        std::cout << it->first << ": ";
                        it->second.print();
                    if (std::next(it) != map.end() )
                        std::cout << ", ";
                    }
                    std::cout << "}";
                },
                [this] (const std::monostate& mono) {
                    std::cout << "null";
                },
                [this] (const bool value) {
                    std::cout << (value ? "true":"false");
                },
                [this] (const auto& value) {
                    std::cout << value;
                }
            }, inner);
    
    }
    
    char JSONObject::unescape_char(char ch) {
        switch (ch) {
            case 'n' : return '\n';
            case 'r' : return '\r';
            case '0' : return '\0';
            case 't' : return '\t';
            case 'v' : return '\v';
            case 'f' : return '\f';
            case 'b' : return '\b';
            case 'a' : return '\a';
            default: return ch;
        }
    }
}