#include "expression.hpp"
#include "parser.hpp"
#include <QDebug>

Liquid::Expression Liquid::Expression::parse(const QStringRef &input)
{
    // Literals
    if (input == "nil" || input == "null" || input == "") {
        return Type::Nil;
    } else if (input == "true") {
        return Type::BooleanTrue;
    } else if (input == "false") {
        return Type::BooleanFalse;
    }
    
    Parser parser(input);
    if (parser.look(Token::Type::EndOfString, 1)) {
        if (parser.look(Token::Type::String)) {
            return parser.consume().toString();
        } else if (parser.look(Token::Type::NumberInt)) {
            return parser.consume().toInt();
        } else if (parser.look(Token::Type::NumberFloat)) {
            return parser.consume().toDouble();
        }
    }
    
    Expression exp(Type::Lookup);
    for (;;) {
        const QStringRef key = parser.consume(Token::Type::Id);
        Expression sub(Type::LookupKey);
        sub.setLookupKey(key.toString());
        exp.addLookup(sub);
        
        if (parser.look(Token::Type::Dot)) {
            (void)parser.consume();
            continue;
        }
        
        break;
    }
    
    return exp;
}



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
    
    SECTION("Parse") {
        QString input = "true";
        Liquid::Expression exp = Liquid::Expression::parse(&input);
        CHECK(exp.toBool());
        input = "false";
        exp = Liquid::Expression::parse(&input);
        CHECK_FALSE(exp.toBool());
        input = "nil";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNil());
        input = "null";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNil());
        input = "";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNil());
        input = "32";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isInt());
        CHECK(exp.toInt() == 32);
        input = "32.49";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isFloat());
        CHECK(exp.toInt() == 32);
        CHECK(exp.toFloat() == 32.49);
        input = "-32";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isInt());
        CHECK(exp.toInt() == -32);
        CHECK(exp.toFloat() == -32);
        input = "-32.49";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isFloat());
        CHECK(exp.toInt() == -32);
        CHECK(exp.toFloat() == -32.49);
        input = "-32.";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isFloat());
        CHECK(exp.toInt() == -32);
        CHECK(exp.toFloat() == -32);
        input = "\"hello\"";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isString());
        CHECK(exp.toString() == "hello");
        input = "'hello'";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isString());
        CHECK(exp.toString() == "hello");
    }
    
    SECTION("LookupKey") {
        QString input = "name";
        Liquid::Expression exp = Liquid::Expression::parse(&input);
        REQUIRE(exp.isLookup());
        REQUIRE(exp.lookups().size() == 1);
        CHECK(exp.lookups()[0].isLookupKey());
        CHECK(exp.lookups()[0].lookupKey() == "name");
    }

    SECTION("MultipleLookupKey") {
        QString input = "first.second.third.fourth.fifth";
        Liquid::Expression exp = Liquid::Expression::parse(&input);
        REQUIRE(exp.isLookup());
        REQUIRE(exp.lookups().size() == 5);
        CHECK(exp.lookups()[0].isLookupKey());
        CHECK(exp.lookups()[0].lookupKey() == "first");
        CHECK(exp.lookups()[1].isLookupKey());
        CHECK(exp.lookups()[1].lookupKey() == "second");
        CHECK(exp.lookups()[2].isLookupKey());
        CHECK(exp.lookups()[2].lookupKey() == "third");
        CHECK(exp.lookups()[3].isLookupKey());
        CHECK(exp.lookups()[3].lookupKey() == "fourth");
        CHECK(exp.lookups()[4].isLookupKey());
        CHECK(exp.lookups()[4].lookupKey() == "fifth");
    }

}

#endif
