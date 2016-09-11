#ifndef LIQUID_BREAK_HPP
#define LIQUID_BREAK_HPP

#include "node.hpp"

namespace Liquid {
    
    class BreakTag : public TagNode {
    public:
        BreakTag(const QStringRef& tagName, const QStringRef& markup) : TagNode(tagName, markup) {}
        
        virtual QString render(Context& ctx) override;
    };
}

#endif
