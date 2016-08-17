#include "context.hpp"
#include <QDebug>




#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Context") {
    
    SECTION("String") {
        Liquid::Context c("test");
        CHECK(c.isString());
        CHECK(c.toString() == "test");
        CHECK_FALSE(c.toBool());
        CHECK(c.toInt() == 0);
        CHECK(c.toFloat() == 0);
        CHECK(c.isTruthy());
    }
    
    SECTION("Boolean") {
        Liquid::Context c1(true);
        CHECK(c1.isBoolean());
        CHECK(c1.toBool());
        Liquid::Context c2(false);
        CHECK(c2.isBoolean());
        CHECK_FALSE(c2.toBool());
        CHECK(c1.toInt() == 0);
        CHECK(c1.toFloat() == 0);
        CHECK(c2.toInt() == 0);
        CHECK(c2.toFloat() == 0);
        CHECK(c1.isTruthy());
        CHECK_FALSE(c2.isTruthy());
    }
    
    SECTION("Number") {
        Liquid::Context c1(23);
        CHECK(c1.isNumber());
        CHECK(c1.toInt() == 23);
        CHECK(c1.toFloat() == 23);
        CHECK(c1.toBool() == false);
        Liquid::Context c2(23.832);
        CHECK(c2.isNumber());
        CHECK(c2.toInt() == 23);
        CHECK(c2.toFloat() == 23.832);
        CHECK(c2.toBool() == false);
        CHECK(c1.isTruthy());
        CHECK(c2.isTruthy());
    }
    
    SECTION("Nil") {
        Liquid::Context c;
        CHECK(c.isNil());
        CHECK(c.toInt() == 0);
        CHECK(c.toString() == "");
        CHECK(c.toFloat() == 0);
        CHECK(c.toBool() == false);
        CHECK_FALSE(c.isTruthy());
    }
}

#endif
