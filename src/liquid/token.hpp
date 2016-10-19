#ifndef LIQUID_TOKEN_HPP
#define LIQUID_TOKEN_HPP

#include "string.hpp"

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
            DotDot,
            EndOfString,
        };
        
        static String typeToString(Type type) {
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
                case Type::DotDot: return "DotDot";
                case Type::EndOfString: return "EndOfString";
                default: return "";
            }
        }
        
        Token()
            : type_(Type::Invalid)
        {
        }
        
        Token(Type type, const StringRef& value)
            : type_(type)
            , value_(value)
        {
        }
        
        Type type() const {
            return type_;
        }
        
        const StringRef& value() const {
            return value_;
        }
        
        bool isValid() const {
            return type_ != Type::Invalid;
        }
        
    private:
        Type type_;
        StringRef value_;
    };
    
    const extern Token kTokenInvalid;
    
}

#endif
