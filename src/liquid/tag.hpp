#ifndef LIQUID_TAG_HPP
#define LIQUID_TAG_HPP

#include <functional>
#include "qstringhash.hpp"
#include "node.hpp"

namespace Liquid {
    
    class Context;
    class Tokenizer;
    
    using TagHandler = std::function<NodePtr(const Context& context, const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer)>;
    using TagHash = std::unordered_map<QString, TagHandler, QStringHash>;
    
}

#endif
