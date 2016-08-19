#include "expression.hpp"
#include <QDebug>



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Expression") {
    
    SECTION("String") {
        Liquid::Expression c("test");
        CHECK(c.isString());
        CHECK(c.toString() == "test");
        Liquid::Expression c2 = "Hello World";
        CHECK(c2.isString());
    }
    
    SECTION("Boolean") {
        Liquid::Expression c1(true);
        CHECK(c1.isBoolean());
        CHECK(c1.toBool());
        Liquid::Expression c2(false);
        CHECK(c2.isBoolean());
        CHECK_FALSE(c2.toBool());
        Liquid::Expression c3 = true;
        CHECK(c3.isBoolean());
        CHECK(c3.toBool() == true);
        Liquid::Expression c4 = false;
        CHECK(c4.isBoolean());
        CHECK(c4.toBool() == false);
    }
    
    SECTION("Number") {
        Liquid::Expression c1(23);
        CHECK(c1.isNumber());
        CHECK(c1.toInt() == 23);
        Liquid::Expression c2(23.832);
        CHECK(c2.isNumber());
        CHECK(c2.toFloat() == 23.832);
        Liquid::Expression c3 = 85;
        CHECK(c3.isNumber());
        CHECK(c3.type() == Liquid::Expression::Type::NumberInt);
        Liquid::Expression c4 = 85.32;
        CHECK(c4.isNumber());
        CHECK(c4.type() == Liquid::Expression::Type::NumberFloat);
    }
    
    SECTION("Copy") {
        Liquid::Expression c1(98);
        Liquid::Expression c2 = c1;
        Liquid::Expression c3(c2);
        CHECK(c1.type() == Liquid::Expression::Type::NumberInt);
        CHECK(c1.type() == c2.type());
        CHECK(c2.type() == c3.type());
        CHECK(c1.toInt() == 98);
        CHECK(c2.toInt() == 98);
        CHECK(c3.toInt() == 98);
    }

}

#endif
