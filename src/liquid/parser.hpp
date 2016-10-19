#ifndef LIQUID_PARSER_HPP
#define LIQUID_PARSER_HPP

#include "lexer.hpp"

namespace Liquid {

    class Parser {
    public:
        Parser(const StringRef& input);
        
        void jump(size_t position) {
            pos_ = position;
        }
        
        StringRef consume() {
            return consume(nullptr);
        }
        
        StringRef consume(Token::Type type) {
            return consume(&type);
        }
        
        bool consume(Token::Type type, StringRef& value);
        
        bool consumeId(const String& name);
        
        bool look(Token::Type type, size_t ahead = 0);

    private:
        std::vector<Token> tokens_;
        size_t pos_;

        const Token& tokenAt(size_t position) const;
        StringRef consume(const Token::Type* type);
    };

}

#endif
