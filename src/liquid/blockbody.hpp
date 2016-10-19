#ifndef LIQUID_BLOCKBODY_HPP
#define LIQUID_BLOCKBODY_HPP

#include <vector>
#include <functional>
#include "string.hpp"
#include "node.hpp"

namespace Liquid {
    
    class Tokenizer;
    class Context;

    class BlockBody {
    public:
        using UnknownTagHandler = std::function<void(const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer)>;
        static void defaultUnknownTagHandler(const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer);
        
        void parse(const Context& context, Tokenizer& tokenizer, const UnknownTagHandler unknownTagHandler = defaultUnknownTagHandler);
        
        String render(Context& context);
        
    private:
        std::vector<NodePtr> nodes_;
    };
    
}

#endif
