#ifndef LIQUID_CONTINUE_HPP
#define LIQUID_CONTINUE_HPP

#include "node.hpp"

namespace Liquid {
    
    class ContinueTag : public TagNode {
    public:
        ContinueTag(const Context& context, const StringRef& tagName, const StringRef& markup)
            : TagNode(context, tagName, markup)
        {}
        
        virtual String render(Context& ctx) override;
    };
}

#endif
