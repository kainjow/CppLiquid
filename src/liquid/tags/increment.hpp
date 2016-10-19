#ifndef LIQUID_INCREMENT_HPP
#define LIQUID_INCREMENT_HPP

#include "node.hpp"

namespace Liquid {
    
    class IncrementTag : public TagNode {
    public:
        IncrementTag(const Context& context, const StringRef& tagName, const StringRef& markup);
        
        virtual String render(Context& context) override;
        
    private:
        StringRef to_;
    };

}

#endif
