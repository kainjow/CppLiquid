#ifndef LIQUID_VARIABLE_HPP
#define LIQUID_VARIABLE_HPP

#include "expression.hpp"

namespace Liquid {

    class Variable {
    public:
        Variable(const Expression& exp)
            : exp_(exp)
        {
        }
        
        const Context& evaluate(const Context& context) const;

    private:
        const Expression exp_;
    };

}

#endif
