#ifndef LIQUID_CYCLE_HPP
#define LIQUID_CYCLE_HPP

#include "node.hpp"
#include "expression.hpp"

namespace Liquid {
    
    class CycleTag : public TagNode {
    public:
        CycleTag(const Context& context, const StringRef& tagName, const StringRef& markup);
        
        virtual String render(Context& context) override;
        
    private:
        Expression nameExpression_;
        String nameString_;
        std::vector<Expression> expressions_;
        bool nameIsExpression_;
    };

}

#endif
