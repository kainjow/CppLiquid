#include "data.hpp"
#include <QDebug>

const Liquid::Data Liquid::kNilData{Data::Type::Nil};
const Liquid::Data Liquid::kTrueData{Data::Type::BooleanTrue};
const Liquid::Data Liquid::kFalseData{Data::Type::BooleanFalse};



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Data") {
    
    SECTION("String") {
        Liquid::Data c("test");
        CHECK(c.isString());
        CHECK(c.toString() == "test");
        CHECK_FALSE(c.toBool());
        CHECK(c.toInt() == 0);
        CHECK(c.toFloat() == 0);
        CHECK(c.isTruthy());
        Liquid::Data c2 = "Hello World";
        CHECK(c2.isString());
    }
    
    SECTION("Boolean") {
        Liquid::Data c1(true);
        CHECK(c1.isBoolean());
        CHECK(c1.toBool());
        CHECK(c1.toInt() == 0);
        CHECK(c1.toFloat() == 0);
        CHECK(c1.toString() == "true");
        CHECK(c1.isTruthy());
        Liquid::Data c2(false);
        CHECK(c2.isBoolean());
        CHECK_FALSE(c2.toBool());
        CHECK(c2.toInt() == 0);
        CHECK(c2.toFloat() == 0);
        CHECK(c2.toString() == "false");
        CHECK_FALSE(c2.isTruthy());
        Liquid::Data c3 = true;
        CHECK(c3.isBoolean());
        CHECK(c3.toBool() == true);
        Liquid::Data c4 = false;
        CHECK(c4.isBoolean());
        CHECK(c4.toBool() == false);
    }
    
    SECTION("Number") {
        Liquid::Data c1(23);
        CHECK(c1.isNumber());
        CHECK(c1.toInt() == 23);
        CHECK(c1.toFloat() == 23);
        CHECK(c1.toBool() == false);
        CHECK(c1.isTruthy());
        CHECK(c1.toString() == "23");
        Liquid::Data c2(23.832);
        CHECK(c2.isNumber());
        CHECK(c2.toInt() == 23);
        CHECK(c2.toFloat() == 23.832);
        CHECK(c2.toBool() == false);
        CHECK(c2.isTruthy());
        CHECK(c2.toString() == "23.832");
        Liquid::Data c3 = 85;
        CHECK(c3.isNumber());
        CHECK(c3.type() == Liquid::Data::Type::NumberInt);
        Liquid::Data c4 = 85.32;
        CHECK(c4.isNumber());
        CHECK(c4.type() == Liquid::Data::Type::NumberFloat);
    }
    
    SECTION("Nil") {
        Liquid::Data c;
        CHECK(c.isNil());
        CHECK(c.toInt() == 0);
        CHECK(c.toString() == "");
        CHECK(c.toFloat() == 0);
        CHECK(c.toBool() == false);
        CHECK_FALSE(c.isTruthy());
        Liquid::Data c2 = nullptr;
        CHECK(c2.isNil());
    }
    
    SECTION("Copy") {
        Liquid::Data c1(98);
        Liquid::Data c2 = c1;
        Liquid::Data c3(c2);
        CHECK(c1.type() == Liquid::Data::Type::NumberInt);
        CHECK(c1.type() == c2.type());
        CHECK(c2.type() == c3.type());
        CHECK(c1.toInt() == 98);
        CHECK(c2.toInt() == 98);
        CHECK(c3.toInt() == 98);
    }
    
    SECTION("Array") {
        Liquid::Data c = Liquid::Data::Type::Array;
        CHECK(c.isArray());
        CHECK(c.size() == 0);
        c.push_back("Hello");
        c.push_back("World");
        CHECK(c.size() == 2);
        CHECK(c.at(0) == "Hello");
        CHECK(c.at(1) == "World");
    }
    
    SECTION("Hash") {
        Liquid::Data c = Liquid::Data::Type::Hash;
        CHECK(c.isHash());
        CHECK(c.size() == 0);
        c.insert("fname", "Steve");
        c.insert("lname", "Jobs");
        CHECK(c.size() == 2);
        CHECK(c["fname"] == "Steve");
        CHECK(c["lname"] == "Jobs");
        CHECK(c["test"] == nullptr);
    }
    
    SECTION("Drop") {
        Liquid::Data c = Liquid::Data::Type::Hash;
        c.insert("drop", std::make_shared<Liquid::Drop>());
        CHECK(c["drop"].isDrop());
    }

}

#endif
