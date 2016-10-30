#include "lexer.hpp"
#include "stringscanner.hpp"
#include "stringutils.hpp"
#include "error.hpp"

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
        {'=', Liquid::Token::Type::Equal},
    };
    const Liquid::String kDotDot = "..";
    const std::vector<Liquid::String> comparisonOperators = {"==", "!=", "<>", "<=", ">=", "<", ">", "contains"};
}

std::vector<Liquid::Token> Liquid::Lexer::tokenize(const StringRef& input)
{
    std::vector<Liquid::Token> tokens;
    StringScanner ss(rtrim(input));
    StringRef tok;
    bool scannedComparisonOperator;

    while (!ss.eof()) {
        (void)ss.skipWhitespace();
        
        scannedComparisonOperator = false;
        for (const auto& op : comparisonOperators) {
            if (ss.scanString(op, &tok)) {
                tokens.emplace_back(Token::Type::Comparison, tok);
                scannedComparisonOperator = true;
                break;
            }
        }
        if (scannedComparisonOperator) {
            continue;
        }

        tok = ss.scanStringLiteral();
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::String, tok);
            continue;
        }

        tok = ss.scanFloat();
        if (!tok.isNull()) {
            if (tok.at(tok.size() - 1) == '.' && ss.peekch() == ".") {
                // This is actually an int in a range, so continue processing.
                ss.advance(-tok.size());
            } else {
                tokens.emplace_back(Token::Type::NumberFloat, tok);
                continue;
            }
        }

        tok = ss.scanInt();
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::NumberInt, tok);
            continue;
        }

        tok = ss.scanIdentifier();
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::Id, tok);
            continue;
        }

        if (ss.scanString(kDotDot, &tok)) {
            tokens.emplace_back(Token::Type::DotDot, tok);
            continue;
        }
        
        tok = ss.getch();
        if (!tok.isNull()) {
            const auto it = kSpecials.find(static_cast<char>(tok.at(0)));
            if (it != kSpecials.end()) {
                tokens.emplace_back(it->second, tok);
            } else {
                throw syntax_error(String("Unexpected character %1").arg(tok.toString()));
            }
        }
    }
    
    tokens.emplace_back(Token::Type::EndOfString, StringRef());
    
    return tokens;
}




#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Lexer") {
    
    SECTION("Empty") {
        Liquid::String input;
        Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 1);
        CHECK(tokens[0].type() == Liquid::Token::Type::EndOfString);
        CHECK(tokens[0].value().toString() == "");
    }

    SECTION("NumberInt") {
        Liquid::String input = "32";
        Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberInt);
        CHECK(tokens[0].value().toString() == "32");
        CHECK(tokens[1].type() == Liquid::Token::Type::EndOfString);
        CHECK(tokens[1].value().toString() == "");
    }

    SECTION("NumberIntNegative") {
        Liquid::String input = "-32";
        Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberInt);
        CHECK(tokens[0].value().toString() == "-32");
    }

    SECTION("NumberFloat1") {
        Liquid::String input = "32.84";
        Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberFloat);
        CHECK(tokens[0].value().toString() == "32.84");
    }

    SECTION("NumberFloat2") {
        Liquid::String input = "32.";
        Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberFloat);
        CHECK(tokens[0].value().toString() == "32.");
    }

    SECTION("NumberFloatNegative") {
        Liquid::String input = "-32.84";
        Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberFloat);
        CHECK(tokens[0].value().toString() == "-32.84");
    }

    SECTION("SingleQuoteString") {
        Liquid::String input = "'Hello'";
        Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::String);
        CHECK(tokens[0].value().toString() == "Hello");
    }

    SECTION("DoubleQuoteString") {
        Liquid::String input = "\"Hello\"";
        Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::String);
        CHECK(tokens[0].value().toString() == "Hello");
    }

    SECTION("DotDot") {
        Liquid::String input = "1..2";
        Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 4);
        CHECK(tokens[0].type() == Liquid::Token::Type::NumberInt);
        CHECK(tokens[0].value().toString().toStdString() == "1");
        CHECK(tokens[1].type() == Liquid::Token::Type::DotDot);
        CHECK(tokens[1].value().toString().toStdString() == "..");
        CHECK(tokens[2].type() == Liquid::Token::Type::NumberInt);
        CHECK(tokens[2].value().toString().toStdString() == "2");
    }

    SECTION("EmptyString") {
        const Liquid::String input = "''";
        const Liquid::StringRef inputRef(&input);
        std::vector<Liquid::Token> tokens = Liquid::Lexer::tokenize(inputRef);
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type() == Liquid::Token::Type::String);
        CHECK(tokens[0].value().toString().toStdString() == "");
        CHECK(tokens[1].type() == Liquid::Token::Type::EndOfString);
    }

}

#endif
