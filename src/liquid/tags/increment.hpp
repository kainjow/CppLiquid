#ifndef LIQUID_INCREMENT_HPP
#define LIQUID_INCREMENT_HPP

#include "node.hpp"

namespace Liquid {
    
    class IncrementTag : public TagNode {
    public:
        IncrementTag(const QStringRef& tagName, const QStringRef& markup);
        
        virtual QString render(Context& context) override;
        
    private:
        QStringRef to_;
    };

}

#endif
