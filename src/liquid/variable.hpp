#ifndef LIQUID_VARIABLE_HPP
#define LIQUID_VARIABLE_HPP

#include <QStringRef>
#include "expression.hpp"
#include "filter.hpp"
#include "data.hpp"

namespace Liquid {
    
    class Parser;
    class Context;
    
    class Variable {
    public:
        Variable() {}
        Variable(const QStringRef& input);
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
