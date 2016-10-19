#ifndef LIQUID_IFCHANGED_HPP
#define LIQUID_IFCHANGED_HPP

#include "block.hpp"

namespace Liquid {
    
    class IfchangedTag : public BlockTag {
    public:
        IfchangedTag(const Context& context, const StringRef& tagName, const StringRef& markup);
        
        virtual String render(Context& context) override;
    };
}

#endif
