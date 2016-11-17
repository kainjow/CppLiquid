#include "stringscanner.hpp"



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::StringScanner") {

    const Liquid::String input = "Hello World";
    const Liquid::StringRef ref{&input};
    Liquid::StringScanner ss(ref);
    CHECK_FALSE(ss.eof());
    CHECK(ss.scanIdentifier() == "Hello");
    ss.skipWhitespace();
    CHECK(ss.scanIdentifier() == "World");
    CHECK(ss.eof());
}

#endif
