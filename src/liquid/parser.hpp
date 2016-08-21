#ifndef LIQUID_PARSER_HPP
#define LIQUID_PARSER_HPP

#include "lexer.hpp"
#include <QDebug>

namespace Liquid {

    class Parser {
    public:
        Parser(const QStringRef& input);
        
        void jump(int position) {
            pos_ = position;
        }
        
        QStringRef consume() {
            return consume(nullptr);
        }
        
        QStringRef consume(Token::Type type) {
            return consume(&type);
        }
        
        bool consume(Token::Type type, QStringRef& value);
        
        bool look(Token::Type type, int ahead = 0);

    private:
        std::vector<Token> tokens_;
        int pos_;

        const Token& tokenAt(int position) const;
        QStringRef consume(const Token::Type* type);
    };

}

#endif
