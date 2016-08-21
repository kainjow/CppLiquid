#ifndef LIQUID_VARIABLE_HPP
#define LIQUID_VARIABLE_HPP

#include "expression.hpp"

namespace Liquid {
    
    class Filter {
    public:
        Filter(const QStringRef& name, const std::vector<Expression>& args)
            : name_(name)
            , args_(args)
        {
        }
        
        const QStringRef& name() const {
            return name_;
        }
        
        const std::vector<Expression>& args() const {
            return args_;
        }
        
    private:
        const QStringRef name_;
        std::vector<Expression> args_;
    };

    class Variable {
    public:
        Variable(const QStringRef& input);
        
        const Data& evaluate(const Data& context) const;

    private:
        Expression exp_;
        std::vector<Filter> filters_;
        mutable Data cached_;
    };

}

#endif
