#include "stringutils.hpp"
#include <sstream>
#include <iomanip>
#include "string.hpp"

Liquid::StringRef Liquid::rtrim(const StringRef& input)
{
    int len = input.size();
    while (len > 0 && isSpace(input.at(len - 1))) {
        --len;
    }
    return input.left(len);
}

Liquid::StringRef Liquid::ltrim(const StringRef& input)
{
    int i;
    const int size = input.size();
    for (i = 0; i < size && isSpace(input.at(i)); ++i) {
    }
    return input.mid(i);
}

Liquid::StringRef Liquid::trim(const StringRef& input)
{
    return ltrim(rtrim(input));
}

Liquid::String Liquid::doubleToString(double value, int precision)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    const String str = stream.str();
    const String::value_type separator = '.';
    if (str.indexOf(separator) != String::npos) {
        int offset = str.size() - 1;
        while (!str.isEmpty() && str.at(offset) == '0') {
            --offset;
        }
        if (!str.isEmpty() && str.at(offset) == separator) {
            --offset;
        }
        return str.left(offset + 1);
    }
    return str;
}


#ifdef TESTS

#include "catch.hpp"

namespace {
    inline std::string tostd(const Liquid::StringRef& ref) {
        return ref.toString().toStdString();
    }
}

TEST_CASE("Liquid::StringUtils") {
    
    SECTION("rtrim") {
        Liquid::String input;
        input = "";
        CHECK(tostd(Liquid::rtrim(&input)) == "");
        input = "hello   ";
        CHECK(tostd(Liquid::rtrim(&input)) == "hello");
        input = "   hello";
        CHECK(tostd(Liquid::rtrim(&input)) == "   hello");
        input = "hello \n\r\t";
        CHECK(tostd(Liquid::rtrim(&input)) == "hello");
        input = " \n  \r \t\t\t   ";
        CHECK(tostd(Liquid::rtrim(&input)) == "");
    }

    SECTION("ltrim") {
        Liquid::String input;
        input = "";
        CHECK(tostd(Liquid::ltrim(&input)) == "");
        input = "hello   ";
        CHECK(tostd(Liquid::ltrim(&input)) == "hello   ");
        input = "   hello";
        CHECK(tostd(Liquid::ltrim(&input)) == "hello");
        input = " \n\r\thello";
        CHECK(tostd(Liquid::ltrim(&input)) == "hello");
        input = " \n  \r \t\t\t   ";
        CHECK(tostd(Liquid::ltrim(&input)) == "");
    }

}

#endif
