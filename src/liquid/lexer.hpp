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
        Type type_;
        QStringRef value_;
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
            const QRegularExpressionMatch match = regex.match(input_, pos_, QRegularExpression::NormalMatch, QRegularExpression::AnchoredMatchOption);
            if (!match.isValid()) {
                return QStringRef();
            }
            // Don't use capturedRef() as that's relative to QRegEx's copy of input :(
            const int capturedLen = match.capturedLength();
            if (capturedLen == 0) {
                return QStringRef();
            }
            const QStringRef captured = input_.mid(pos_, capturedLen);
            pos_ += capturedLen;
            return captured;
        }
        
        bool eof() const {
            return pos_ >= input_.size();
        }
        
        QStringRef getch() {
            return input_.mid(pos_, 1);
        }
        
    private:
        const QStringRef& input_;
        int pos_;
    };

    class Lexer {
    public:
        static std::vector<Token> tokenize(const QStringRef& input);
    };

}

#endif
