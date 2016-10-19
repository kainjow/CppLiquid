#ifndef LIQUID_LEXER_HPP
#define LIQUID_LEXER_HPP

#include "string.hpp"
#include "token.hpp"
#include <vector>

namespace Liquid {
    
    class Lexer {
    public:
        static std::vector<Token> tokenize(const StringRef& input);
    };

}

#endif
