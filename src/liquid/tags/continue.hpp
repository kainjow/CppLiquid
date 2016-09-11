#ifndef LIQUID_CONTINUE_HPP
#define LIQUID_CONTINUE_HPP

#include "node.hpp"

namespace Liquid {
    
    class ContinueTag : public TagNode {
    public:
        ContinueTag(const QStringRef& tagName, const QStringRef& markup) : TagNode(tagName, markup) {}
        
        virtual QString render(Context& ctx) override;
    };
}

#endif
