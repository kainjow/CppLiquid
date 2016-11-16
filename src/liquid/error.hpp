#ifndef LIQUID_ERROR_HPP
#define LIQUID_ERROR_HPP

#include <exception>
#include "string.hpp"

namespace Liquid {

    class syntax_error : public std::runtime_error {
    public:
        syntax_error(const String& what_arg) : std::runtime_error(what_arg.toStdString())
        {
        }
    };

}

#endif
