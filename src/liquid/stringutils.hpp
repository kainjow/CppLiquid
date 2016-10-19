#ifndef LIQUID_STRINGUTILS_HPP
#define LIQUID_STRINGUTILS_HPP

#include "string.hpp"

namespace Liquid {

    StringRef rtrim(const StringRef& input);
    StringRef ltrim(const StringRef& input);
    String doubleToString(double value, int precision = 6);
    
    template <typename T>
    bool isSpace(const T ch) {
        return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
    }

}

#endif
