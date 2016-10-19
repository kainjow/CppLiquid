#ifndef LIQUID_NODE_HPP
#define LIQUID_NODE_HPP

#include "variable.hpp"
#include "string.hpp"
#include <memory>

namespace Liquid {
    
    class Context;
    class Tokenizer;
    
    class Node {
    public:
        Node(const Context&) {}
        virtual String render(Context& context) = 0;
    };

    class TextNode : public Node {
    public:
        TextNode(const Context& context, const StringRef& text);
        
        virtual String render(Context&) override;
        
    private:
        const StringRef text_;
    };
    
    class ObjectNode : public Node {
    public:
        ObjectNode(const Context& context, const Variable& var);

        virtual String render(Context& context) override;

    private:
        const Variable var_;
    };
    
    class TagNode : public Node {
    public:
        TagNode(const Context& context, const StringRef& tagName, const StringRef&)
            : Node(context)
            , tagName_(tagName)
        {}
        virtual String render(Context& context) override;
        const StringRef& tagName() {
            return tagName_;
        }
    private:
        const StringRef tagName_;
    };
    
    using NodePtr = std::shared_ptr<Node>;
}

#endif
