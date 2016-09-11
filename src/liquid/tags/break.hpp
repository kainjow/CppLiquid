#ifndef LIQUID_BREAK_HPP
#define LIQUID_BREAK_HPP

#include "node.hpp"

namespace Liquid {
    
    class BreakTag : public TagNode {
    public:
        BreakTag(const Context& context, const QStringRef& tagName, const QStringRef& markup)
            : TagNode(context, tagName, markup)
        {}
        
        virtual QString render(Context& ctx) override;
    };
}

#endif
