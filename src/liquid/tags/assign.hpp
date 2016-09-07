#ifndef LIQUID_ASSIGN_HPP
#define LIQUID_ASSIGN_HPP

#include "node.hpp"

namespace Liquid {
    
    class AssignTag : public Node {
    public:
        AssignTag(Parser& parser);
        
        virtual QString render(Context& ctx) override;
        
    private:
        QStringRef to_;
        Variable from_;
    };
}

#endif
