#include "stringscanner.hpp"



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::StringScanner") {

    Liquid::String input = "Hello World";
    Liquid::StringScanner ss(&input);
    CHECK_FALSE(ss.eof());
    CHECK(ss.scanIdentifier() == "Hello");
    ss.skipWhitespace();
    CHECK(ss.scanIdentifier() == "World");
    CHECK(ss.eof());
}

#endif
