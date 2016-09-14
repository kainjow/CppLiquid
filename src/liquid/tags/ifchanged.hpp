#ifndef LIQUID_IFCHANGED_HPP
#define LIQUID_IFCHANGED_HPP

#include "block.hpp"

namespace Liquid {
    
    class IfchangedTag : public BlockTag {
    public:
        IfchangedTag(const Context& context, const QStringRef& tagName, const QStringRef& markup);
        
        virtual QString render(Context& context) override;
    };
}

#endif
