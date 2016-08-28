#include "parser.hpp"
#include <QDebug>

Liquid::Parser::Parser(const QStringRef& input)
    : tokens_(Lexer::tokenize(input))
    , pos_(0)
{
}

const Liquid::Token& Liquid::Parser::tokenAt(size_t position) const
{
    if (tokens_.empty() || position >= tokens_.size()) {
        return kTokenInvalid;
    }
    return tokens_.at(position);
}


QStringRef Liquid::Parser::consume(const Token::Type* type)
{
    const Token& token = tokenAt(pos_);
    if (type && token.type() != *type) {
        throw QString("Expected %1 but found %2").arg(Token::typeToString(*type)).arg(Token::typeToString(token.type())).toStdString();
    }
    ++pos_;
    return token.value();
}

bool Liquid::Parser::consume(Token::Type type, QStringRef& value)
{
    const Token& token = tokenAt(pos_);
    if (!token.isValid() || token.type() != type) {
        return false;
    }
    ++pos_;
    value = token.value();
    return true;
}

bool Liquid::Parser::look(Token::Type type, size_t ahead)
{
    const Token& token = tokenAt(pos_ + ahead);
    if (!token.isValid()) {
        return false;
    }
    return token.type() == type;
}



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Parser") {

    SECTION("Consume1") {
        QString input = "Hello World";
        Liquid::Parser p(&input);
        CHECK(p.consume() == "Hello");
        CHECK_THROWS_AS(p.consume(Liquid::Token::Type::NumberInt), std::string);
        CHECK(p.consume(Liquid::Token::Type::Id) == "World");
        CHECK(p.consume(Liquid::Token::Type::EndOfString).isNull());
    }

    SECTION("Consume2") {
        QString input = "Hello World";
        Liquid::Parser p(&input);
        QString errmsg;
        QStringRef value;
        CHECK(p.consume(Liquid::Token::Type::Id, value));
        REQUIRE_FALSE(value.isNull());
        CHECK(value.toString() == "Hello");
        CHECK(p.consume(Liquid::Token::Type::Id, value));
        REQUIRE_FALSE(value.isNull());
        CHECK(value.toString() == "World");
        CHECK(p.consume(Liquid::Token::Type::EndOfString, value));
    }

    SECTION("Look") {
        QString input = "Hello World";
        Liquid::Parser p(&input);
        QString errmsg;
        QStringRef value;
        CHECK(p.look(Liquid::Token::Type::Id));
        CHECK(p.look(Liquid::Token::Type::Id, 1));
        CHECK(p.look(Liquid::Token::Type::EndOfString, 2));
        CHECK_FALSE(p.look(Liquid::Token::Type::NumberInt));
        CHECK_FALSE(p.look(Liquid::Token::Type::NumberInt, 1));
        CHECK_FALSE(p.look(Liquid::Token::Type::NumberInt, 100));
    }

    SECTION("ConsumeEmptyString") {
        QString input = "\"Hello\"";
        Liquid::Parser p1(&input);
        CHECK(p1.consume(Liquid::Token::Type::String) == "Hello");
        CHECK(p1.consume(Liquid::Token::Type::EndOfString).isNull());

        input = "'Hello'";
        Liquid::Parser p2(&input);
        CHECK(p2.consume(Liquid::Token::Type::String) == "Hello");
        CHECK(p2.consume(Liquid::Token::Type::EndOfString).isNull());
}
}

#endif
