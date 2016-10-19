#ifndef LIQUID_VARIABLE_HPP
#define LIQUID_VARIABLE_HPP

#include "string.hpp"
#include "expression.hpp"
#include "filter.hpp"
#include "data.hpp"

namespace Liquid {
    
    class Parser;
    class Context;
    
    class Variable {
    public:
        Variable() {}
        Variable(const StringRef& input);
        Variable(Parser& parser);
        
        const Data& evaluate(const Context& context) const;

    private:
        Expression exp_;
        std::vector<Filter> filters_;
        mutable Data cached_;
        
        void parse(Parser& parser);
    };

}

#endif
