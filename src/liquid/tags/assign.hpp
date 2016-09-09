#ifndef LIQUID_ASSIGN_HPP
#define LIQUID_ASSIGN_HPP

#include "node.hpp"

namespace Liquid {
    
    class AssignTag : public TagNode {
    public:
        AssignTag(const QStringRef& tagName, const QStringRef& markup);
        
        virtual QString render(Context& ctx) override;
        
    private:
        QStringRef to_;
        Variable from_;
    };
}

#endif
