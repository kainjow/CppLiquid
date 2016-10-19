#ifndef LIQUID_BREAK_HPP
#define LIQUID_BREAK_HPP

#include "node.hpp"

namespace Liquid {
    
    class BreakTag : public TagNode {
    public:
        BreakTag(const Context& context, const StringRef& tagName, const StringRef& markup)
            : TagNode(context, tagName, markup)
        {}
        
        virtual String render(Context& ctx) override;
    };
}

#endif
