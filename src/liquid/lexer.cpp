#include "lexer.hpp"
#include <QDebug>
#include <unordered_map>

namespace {
    const QRegularExpression kWhitespace("\\s*");
    
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
    
    const QRegularExpression kIdentifier("[a-zA-Z_][\\w-]*");
    const QRegularExpression kSingleStringLiteral("'[^\']*'");
    const QRegularExpression kDoubleStringLiteral("\"[^\"]*\"");
    const QRegularExpression kNumberLiteral("-?\\d+(\\.\\d+)?");
    const QRegularExpression kDotDot("\\.\\.");
    const QRegularExpression kComparisonOperator("==|!=|<>|<=?|>=?|contains");
}

std::vector<Liquid::Token> Liquid::Lexer::tokenize(const QStringRef& input)
{
    std::vector<Liquid::Token> tokens;
    
    // Strip whitespace from right side
    QStringRef str = input;
    int len = str.size();
    while (str.at(len - 1).isSpace()) {
        --len;
    }
    
    StringScanner ss(input.left(len));
    QStringRef tok;
    
    while (!ss.eof()) {
        ss.skip(kWhitespace);
        
        tok = ss.scan(kComparisonOperator);
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::Comparison, tok);
            continue;
        }

        tok = ss.scan(kSingleStringLiteral);
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::String, tok);
            continue;
        }

        tok = ss.scan(kDoubleStringLiteral);
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::String, tok);
            continue;
        }

        tok = ss.scan(kNumberLiteral);
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::Number, tok);
            continue;
        }

        tok = ss.scan(kIdentifier);
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::Id, tok);
            continue;
        }

        tok = ss.scan(kDotDot);
        if (!tok.isNull()) {
            tokens.emplace_back(Token::Type::Dotdot, tok);
            continue;
        }
        
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
    
    return tokens;
}




#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::StringScanner") {

    QString input = "Hello World";
    Liquid::StringScanner ss(&input);
    CHECK_FALSE(ss.eof());
    CHECK(ss.scan("\\w+") == "Hello");
    CHECK(ss.scan("\\w+").isNull());
    CHECK(ss.scan("\\s+") == " ");
    CHECK(ss.scan("\\s+").isNull());
    CHECK(ss.scan("\\w+") == "World");
    CHECK(ss.scan("\\w+").isNull());
    CHECK(ss.eof());
}

TEST_CASE("Liquid::Lexer") {
    
    

}

#endif
