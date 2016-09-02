#ifndef LIQUID_LEXER_HPP
#define LIQUID_LEXER_HPP

#include <QDebug>
#include <QStringRef>
#include <vector>
#include "token.hpp"

namespace Liquid {
    
    class Lexer {
    public:
        static std::vector<Token> tokenize(const QStringRef& input);
    };

}

#endif
