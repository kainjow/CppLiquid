#ifndef LIQUID_NODE_HPP
#define LIQUID_NODE_HPP

#include "variable.hpp"
#include <QStringRef>

namespace Liquid {
    
    class Context;
    class Tokenizer;
    class Parser;
    
    class Node {
    public:
        virtual QString render(Context& context) const = 0;
    };

    class TextNode : public Node {
    public:
        TextNode(const QStringRef& text);
        
        virtual QString render(Context&) const override;
        
    private:
        const QStringRef text_;
    };
    
    class ObjectNode : public Node {
    public:
        ObjectNode(const Variable& var);

        virtual QString render(Context& context) const override;

    private:
        const Variable var_;
    };
    
    class BlockTag : public Node {
    public:
        BlockTag(Tokenizer& tokenizer);
    };

    class AssignTag : public Node {
    public:
        AssignTag(Parser& parser);
        
        virtual QString render(Context& ctx) const override;
        
    private:
        QStringRef to_;
        Variable from_;
    };
    
    using NodePtr = std::shared_ptr<Node>;
}

#endif
