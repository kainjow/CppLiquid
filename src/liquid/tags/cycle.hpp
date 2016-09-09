#ifndef LIQUID_CYCLE_HPP
#define LIQUID_CYCLE_HPP

#include "node.hpp"
#include "expression.hpp"

namespace Liquid {
    
    class CycleTag : public TagNode {
    public:
        CycleTag(const QStringRef& tagName, const QStringRef& markup);
        
        virtual QString render(Context& context) override;
        
    private:
        Expression nameExpression_;
        QString nameString_;
        std::vector<Expression> expressions_;
        bool nameIsExpression_;
    };

}

#endif
