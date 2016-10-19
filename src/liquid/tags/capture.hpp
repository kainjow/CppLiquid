#ifndef LIQUID_CAPTURE_HPP
#define LIQUID_CAPTURE_HPP

#include "block.hpp"

namespace Liquid {
    
    class CaptureTag : public BlockTag {
    public:
        CaptureTag(const Context& context, const StringRef& tagName, const StringRef& markup);
        
        virtual String render(Context& context) override;
        
    private:
        StringRef to_;
    };
}

#endif
