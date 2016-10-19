#ifndef LIQUID_TAG_HPP
#define LIQUID_TAG_HPP

#include <functional>
#include "node.hpp"

namespace Liquid {
    
    class Context;
    class Tokenizer;
    
    using TagHandler = std::function<NodePtr(const Context& context, const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer)>;
    using TagHash = StringKeyUnorderedMap<TagHandler>;
    
}

#endif
