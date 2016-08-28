#ifndef LIQUID_PARSER_HPP
#define LIQUID_PARSER_HPP

#include "lexer.hpp"
#include <QDebug>

namespace Liquid {

    class Parser {
    public:
        Parser(const QStringRef& input);
        
        void jump(size_t position) {
            pos_ = position;
        }
        
        QStringRef consume() {
            return consume(nullptr);
        }
        
        QStringRef consume(Token::Type type) {
            return consume(&type);
        }
        
        bool consume(Token::Type type, QStringRef& value);
        
        bool look(Token::Type type, size_t ahead = 0);

    private:
        std::vector<Token> tokens_;
        size_t pos_;

        const Token& tokenAt(size_t position) const;
        QStringRef consume(const Token::Type* type);
    };

}

#endif
