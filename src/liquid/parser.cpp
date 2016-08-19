#include "parser.hpp"
#include <QDebug>

Liquid::Parser::Parser(const QStringRef& input)
    : tokens_(Lexer::tokenize(input))
    , pos_(0)
{
}

const Liquid::Token& Liquid::Parser::tokenAt(int position) const
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
        throw QString("Expected %1 but found %2").arg(static_cast<int>(*type)).arg(static_cast<int>(token.type()));
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

bool Liquid::Parser::look(Token::Type type, int ahead)
{
    const Token& token = tokenAt(pos_ + ahead);
    if (!token.isValid()) {
        return false;
    }
    return token.type() == type;
}

QString Liquid::Parser::expression()
{
    const Token& token = tokenAt(pos_);
    if (token.type() == Token::Type::Id) {
        return variable_signature();
    } else if (token.type() == Token::Type::String || token.type() == Token::Type::NumberInt || token.type() == Token::Type::NumberFloat) {
        return consume().toString();
    } else if (token.type() == Token::Type::OpenRound) {
        (void)consume();
        QString first = expression();
        consume(Token::Type::Dotdot);
        QString last = expression();
        consume(Token::Type::CloseRound);
        return QString("(%1..%2)").arg(first).arg(last);
    } else {
        throw QString("%1 is not a valid expression").arg(token.value().toString());
    }
}

QString Liquid::Parser::argument()
{
    QString str;
    // might be a keyword argument (identifier: expression)
    if (look(Token::Type::Id) && look(Token::Type::Colon)) {
        str += consume().toString() + consume().toString() + "";
    }
    str += expression();
    return str;
}

QString Liquid::Parser::variable_signature()
{
    QString str = consume(Token::Type::Id).toString();
    while (look(Token::Type::OpenSquare)) {
        str += consume().toString() + expression() + consume(Token::Type::CloseSquare).toString();
    }
    if (look(Token::Type::Dot)) {
        str += consume().toString() + variable_signature();
    }
    return str;
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Parser") {

    SECTION("Consume1") {
        QString input = "Hello World";
        Liquid::Parser p(&input);
        CHECK(p.consume() == "Hello");
        CHECK_THROWS_AS(p.consume(Liquid::Token::Type::NumberInt), QString);
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

}

#endif
