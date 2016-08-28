#ifndef LIQUID_VARIABLE_HPP
#define LIQUID_VARIABLE_HPP

#include "context.hpp"
#include "filter.hpp"

namespace Liquid {
    
    class Variable {
    public:
        Variable(const QStringRef& input);
        
        const Data& evaluate(const Context& context) const;

    private:
        Expression exp_;
        std::vector<Filter> filters_;
        mutable Data cached_;
    };

}

#endif
