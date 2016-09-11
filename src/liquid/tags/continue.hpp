#ifndef LIQUID_CONTINUE_HPP
#define LIQUID_CONTINUE_HPP

#include "node.hpp"

namespace Liquid {
    
    class ContinueTag : public TagNode {
    public:
        ContinueTag(const Context& context, const QStringRef& tagName, const QStringRef& markup)
            : TagNode(context, tagName, markup)
        {}
        
        virtual QString render(Context& ctx) override;
    };
}

#endif
