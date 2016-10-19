#ifndef LIQUID_ASSIGN_HPP
#define LIQUID_ASSIGN_HPP

#include "node.hpp"

namespace Liquid {
    
    class AssignTag : public TagNode {
    public:
        AssignTag(const Context& context, const StringRef& tagName, const StringRef& markup);
        
        virtual String render(Context& ctx) override;
        
    private:
        StringRef to_;
        Variable from_;
    };
}

#endif
