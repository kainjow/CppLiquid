#ifndef LIQUID_TOKEN_HPP
#define LIQUID_TOKEN_HPP

#include <QDebug>
#include <QStringRef>

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
            Equal,
            Comparison,
            String,
            NumberInt,
            NumberFloat,
            Id,
            Dotdot,
            EndOfString,
        };
        
        static QString typeToString(Type type) {
            switch (type) {
                case Type::Invalid: return "Invalid";
                case Type::Pipe: return "Pipe";
                case Type::Dot: return "Dot";
                case Type::Colon: return "Colon";
                case Type::Comma: return "Comma";
                case Type::OpenSquare: return "OpenSquare";
                case Type::CloseSquare: return "CloseSquare";
                case Type::OpenRound: return "OpenRound";
                case Type::CloseRound: return "CloseRound";
                case Type::Question: return "Question";
                case Type::Dash: return "Dash";
                case Type::Equal: return "Equal";
                case Type::Comparison: return "Comparison";
                case Type::String: return "String";
                case Type::NumberInt: return "NumberInt";
                case Type::NumberFloat: return "NumberFloat";
                case Type::Id: return "Identifier";
                case Type::Dotdot: return "DotDot";
                case Type::EndOfString: return "EndOfString";
            }
        }
        
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
    
}

#endif
