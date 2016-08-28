#include "lexer.hpp"
#include "stringutils.hpp"
#include <QDebug>
#include <unordered_map>

const Liquid::Token Liquid::kTokenInvalid;

namespace {
    const std::unordered_map<char, Liquid::Token::Type> kSpecials{
        {'|', Liquid::Token::Type::Pipe},
        {'.', Liquid::Token::Type::Dot},
        {':', Liquid::Token::Type::Colon},
        {',', Liquid::Token::Type::Comma},
        {'[', Liquid::Token::Type::OpenSquare},
        {']', Liquid::Token::Type::CloseSquare},
        {'(', Liquid::Token::Type::OpenRound},
        {')', Liquid::Token::Type::CloseRound},
        {'?', Liquid::Token::Type::Question},
        {'-', Liquid::Token::Type::Dash},
    };
    
    const QRegularExpression kIdentifier("[a-zA-Z_][\\w-]+");
    const QRegularExpression kNumberFloatLiteral("-?\\d+\\.\\d*");
    const QRegularExpression kNumberIntLiteral("-?\\d+");
}

std::vector<Liquid::Token> Liquid::Lexer::tokenize(const QStringRef& input)
{
    std::vector<Liquid::Token> tokens;
    StringScanner ss(rtrim(input));
    QStringRef tok;
    
    while (!ss.eof()) {
        ss.skipWhitespace();
        
        // TODO: Token::Type::Comparison

        tok = ss.scanStringLiteral();
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::String, tok);
            continue;
        }

        tok = ss.scan(kNumberFloatLiteral);
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::NumberFloat, tok);
            continue;
        }

        tok = ss.scan(kNumberIntLiteral);
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::NumberInt, tok);
            continue;
        }

        tok = ss.scan(kIdentifier);
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::Id, tok);
            continue;
        }

        // TODO: Token::Type::Dotdot
        
        tok = ss.getch();
        if (!tok.isNull()) {
            const auto it = kSpecials.find(tok.at(0).toLatin1());
            if (it != kSpecials.end()) {
                tokens.emplace_back(it->second, tok);
            } else {
                qDebug() << "Unexpected character " << tok;
            }
        }
    }
    
    tokens.emplace_back(Token::Type::EndOfString, QStringRef());
    
    return tokens;
}




#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::StringScanner") {

    QString input = "Hello World";
    Liquid::StringScanner ss(&input);
    CHECK_FALSE(ss.eof());
    CHECK(ss.scan(QRegularExpression("\\w+")) == "Hello");
    CHECK(ss.scan(QRegularExpression("\\w+")).isNull());
    CHECK(ss.scan(QRegularExpression("\\s+")) == " ");
    CHECK(ss.scan(QRegularExpression("\\s+")).isNull());
    CHECK(ss.scan(QRegularExpression("\\w+")) == "World");
    CHECK(ss.scan(QRegularExpression("\\w+")).isNull());
    CHECK(ss.eof());
}

TEST_CASE("Liquid::Lexer") {
    
    SECTION("Empty") {
        QString input;
        QStringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 1);
        CHECK(tokens[0].type() == Liquid::Token::Type::EndOfString);
        CHECK(tokens[0].value().toString() == "");
    }

    SECTION("NumberInt") {
        QString input = "32";
        QStringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberInt);
        CHECK(tokens[0].value().toString() == "32");
        CHECK(tokens[1].type() == Liquid::Token::Type::EndOfString);
        CHECK(tokens[1].value().toString() == "");
    }

    SECTION("NumberIntNegative") {
        QString input = "-32";
        QStringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberInt);
        CHECK(tokens[0].value().toString() == "-32");
    }

    SECTION("NumberFloat1") {
        QString input = "32.84";
        QStringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberFloat);
        CHECK(tokens[0].value().toString() == "32.84");
    }

    SECTION("NumberFloat2") {
        QString input = "32.";
        QStringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberFloat);
        CHECK(tokens[0].value().toString() == "32.");
    }

    SECTION("NumberFloatNegative") {
        QString input = "-32.84";
        QStringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberFloat);
        CHECK(tokens[0].value().toString() == "-32.84");
    }

    SECTION("SingleQuoteString") {
        QString input = "'Hello'";
        QStringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::String);
        CHECK(tokens[0].value().toString() == "Hello");
    }

    SECTION("DoubleQuoteString") {
        QString input = "\"Hello\"";
        QStringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::String);
        CHECK(tokens[0].value().toString() == "Hello");
    }
    
}

#endif
