#include "stringutils.hpp"
#include <QDebug>

QStringRef Liquid::rtrim(const QStringRef& input)
{
    int len = input.size();
    while (len > 0 && input.at(len - 1).isSpace()) {
        --len;
    }
    return input.left(len);
}

QStringRef Liquid::ltrim(const QStringRef& input)
{
    int i;
    const int size = input.size();
    for (i = 0; i < size && input.at(i).isSpace(); ++i) {
    }
    return input.mid(i);
}



#ifdef TESTS

#include "catch.hpp"

namespace {
    inline std::string tostd(const QStringRef& ref) {
        return ref.toString().toStdString();
    }
}

TEST_CASE("Liquid::StringUtils") {
    
    SECTION("rtrim") {
        QString input;
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
        QString input;
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
