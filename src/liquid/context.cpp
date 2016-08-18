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
        CHECK(c1.toInt() == 0);
        CHECK(c1.toFloat() == 0);
        CHECK(c1.toString() == "true");
        CHECK(c1.isTruthy());
        Liquid::Context c2(false);
        CHECK(c2.isBoolean());
        CHECK_FALSE(c2.toBool());
        CHECK(c2.toInt() == 0);
        CHECK(c2.toFloat() == 0);
        CHECK(c2.toString() == "false");
        CHECK_FALSE(c2.isTruthy());
    }
    
    SECTION("Number") {
        Liquid::Context c1(23);
        CHECK(c1.isNumber());
        CHECK(c1.toInt() == 23);
        CHECK(c1.toFloat() == 23);
        CHECK(c1.toBool() == false);
        CHECK(c1.isTruthy());
        CHECK(c1.toString() == "23");
        Liquid::Context c2(23.832);
        CHECK(c2.isNumber());
        CHECK(c2.toInt() == 23);
        CHECK(c2.toFloat() == 23.832);
        CHECK(c2.toBool() == false);
        CHECK(c2.isTruthy());
        CHECK(c2.toString() == "23.832");
    }
    
    SECTION("Nil") {
        Liquid::Context c;
        CHECK(c.isNil());
        CHECK(c.toInt() == 0);
        CHECK(c.toString() == "");
        CHECK(c.toFloat() == 0);
        CHECK(c.toBool() == false);
        CHECK_FALSE(c.isTruthy());
        
        Liquid::Context c2 = nullptr;
        CHECK(c2.isNil());
    }
    
    SECTION("Copy") {
        Liquid::Context c1(98);
        Liquid::Context c2 = c1;
        Liquid::Context c3(c2);
        CHECK(c1.type() == Liquid::Context::Type::NumberInt);
        CHECK(c1.type() == c2.type());
        CHECK(c2.type() == c3.type());
        CHECK(c1.toInt() == 98);
        CHECK(c2.toInt() == 98);
        CHECK(c3.toInt() == 98);
    }
    
    SECTION("Array") {
        Liquid::Context c = Liquid::Context::Type::Array;
        CHECK(c.isArray());
        CHECK(c.size() == 0);
        c.push_back("Hello");
        c.push_back("World");
        CHECK(c.size() == 2);
        CHECK(c.at(0) == "Hello");
        CHECK(c.at(1) == "World");
    }
    
}

#endif
