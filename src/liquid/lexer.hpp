#ifndef LIQUID_LEXER_HPP
#define LIQUID_LEXER_HPP

#include <QDebug>
#include <QString>
#include <QRegularExpression>
#include <vector>

namespace Liquid {

    class Token {
    public:
        enum class Type {
            Pipe,
            Dot,
            Colon,
            Comma,
            OpenSquare,
            CloseSquare,
            OpenRound,
            CloseRound,
            Question,
            Dash,
            Comparison,
            String,
            Number,
            Id,
            Dotdot,
        };
        
        Token(Type type, const QStringRef& value)
            : type_(type)
            , value_(value)
        {
        }
        
        Type type() const {
            return type_;
        }
        
        const QStringRef& value() const {
            return value_;
        }
        
    private:
        const Type type_;
        const QStringRef& value_;
    };
    
    class StringScanner {
    public:
        StringScanner(const QStringRef& input)
            : input_(input)
            , pos_(0)
        {
        }
        
        void skip(const QRegularExpression& regex) {
            (void)scan(regex);
        }
        
        QStringRef scan(const QString& str) {
            return scan(QRegularExpression(str));
        }
        
        QStringRef scan(const QRegularExpression& regex) {
            if (!regex.isValid()) {
                return QStringRef();
            }
            lastMatch_ = regex.match(input_, pos_, QRegularExpression::NormalMatch, QRegularExpression::AnchoredMatchOption);
            if (!lastMatch_.isValid()) {
                return QStringRef();
            }
            const QStringRef captured = lastMatch_.capturedRef();
            pos_ += captured.size();
            return captured;
        }
        
        bool eof() const {
            return pos_ >= input_.size();
        }
        
        QStringRef getch() {
            return input_.mid(pos_, 1);
        }
        
    private:
        QRegularExpressionMatch lastMatch_;
        int pos_;
        const QStringRef& input_;
    };

    class Lexer {
    public:
        std::vector<Token> tokenize(const QStringRef& input);
    };

}

#endif
