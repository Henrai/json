#include <type_traits>
#ifndef __JSON_H__

#include <charconv>
#include <optional>
#include <string_view>
#include <variant>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
namespace Json {


struct JSONObject;

using JSONList = std::vector<JSONObject>;
using JSONMap =  std::unordered_map<std::string, JSONObject>;
struct JSONObject {
    std::variant<
        std::monostate,
        bool,
        int,
        double,
        std::string,
        JSONList,
        JSONMap 
    > inner;
    static std::pair<JSONObject, size_t> parse(std::string_view json);
    static bool canEatToken(std::string_view json, std::string_view token);
    static char unescape_char(char ch);

    template<typename T>
    static std::optional<T> tryParseNum(std::string_view str) {
        T value;
        int n, res = 0;
        if constexpr (std::is_same_v<T, int>) {
            res = sscanf(str.data(), "%d%n", &value, &n);
        } else if constexpr (std::is_same_v<T, double>) {
            res = sscanf(str.data(), "%lf%n", &value, &n);
        }
        if(res && n == str.size()) {
            return value;
        }
        return std::nullopt;
    }

    template <class ...Fs>
    struct overloaded : Fs... {
        using Fs::operator()...;
    };

    template <class ...Fs>
    overloaded(Fs...) -> overloaded<Fs...>;

    // Function to print values of std::variant
    void print() const;
};


} // namespace Json



#endif