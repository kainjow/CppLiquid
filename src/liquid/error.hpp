#ifndef LIQUID_ERROR_HPP
#define LIQUID_ERROR_HPP

#include <exception>
#include "string.hpp"

namespace Liquid {

    class syntax_error : public std::exception {
    public:
        syntax_error(const String& what_arg)
            : what_(what_arg)
        {
        }
        
        virtual const char* what() const noexcept override {
            return what_.toStdString().c_str();
        }
        
    private:
        String what_;
    };

}

#endif
