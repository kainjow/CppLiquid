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
            Invalid,
            
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
            NumberInt,
            NumberFloat,
            Id,
            Dotdot,
            EndOfString,
        };
        
        Token()
            : type_(Type::Invalid)
        {
        }
        
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
        
        bool isValid() const {
            return type_ != Type::Invalid;
        }
        
    private:
        Type type_;
        QStringRef value_;
    };
    
    const extern Token kTokenInvalid;
    
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
            QStringRef str = input_.mid(pos_, 1);
            ++pos_;
            return str;
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
