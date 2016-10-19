#ifndef LIQUID_DECREMENT_HPP
#define LIQUID_DECREMENT_HPP

#include "node.hpp"

namespace Liquid {
    
    class DecrementTag : public TagNode {
    public:
        DecrementTag(const Context& context, const StringRef& tagName, const StringRef& markup);
        
        virtual String render(Context& context) override;
        
    private:
        StringRef to_;
    };

}

#endif
