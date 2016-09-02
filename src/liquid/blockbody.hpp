#ifndef LIQUID_BLOCKBODY_HPP
#define LIQUID_BLOCKBODY_HPP

#include <vector>
#include <QString>
#include "node.hpp"

namespace Liquid {
    
    class Tokenizer;
    class Context;

    class BlockBody {
    public:
        BlockBody();
        BlockBody(Tokenizer& tokenizer);
        
        QString render(Context& context);
        
    private:
        std::vector<NodePtr> nodes_;
    };
    
}

#endif
