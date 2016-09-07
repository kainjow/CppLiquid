#ifndef LIQUID_DECREMENT_HPP
#define LIQUID_DECREMENT_HPP

#include "node.hpp"

namespace Liquid {
    
    class DecrementTag : public TagNode {
    public:
        DecrementTag(const QStringRef& tagName, Parser& parser);
        
        virtual QString render(Context& context) override;
        
    private:
        QStringRef to_;
    };

}

#endif
