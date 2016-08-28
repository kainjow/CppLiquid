#include "stringscanner.hpp"
#include <QDebug>



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::StringScanner") {

    QString input = "Hello World";
    Liquid::StringScanner ss(&input);
    CHECK_FALSE(ss.eof());
    CHECK(ss.scanIdentifier() == "Hello");
    ss.skipWhitespace();
    CHECK(ss.scanIdentifier() == "World");
    CHECK(ss.eof());
}

#endif
