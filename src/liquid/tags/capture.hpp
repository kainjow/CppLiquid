#ifndef LIQUID_CAPTURE_HPP
#define LIQUID_CAPTURE_HPP

#include "block.hpp"

namespace Liquid {
    
    class CaptureTag : public BlockTag {
    public:
        CaptureTag(const QStringRef& tagName, Parser& parser);
        
        virtual QString render(Context& context) override;
        
    private:
        QStringRef to_;
    };
}

#endif
