#ifndef LIQUID_COMPONENT_HPP
#define LIQUID_COMPONENT_HPP

#include "context.hpp"
#include "variable.hpp"

namespace Liquid {
    
    class Component {
    public:
        enum class Type {
            Text,
            Object,
            Tag,
        };
        Component(Type theType, const QStringRef& theText, const QStringRef& theInnerText)
            : type(theType)
            , text(theText)
            , innerText(theInnerText)
        {
        }
        Type type;
        QStringRef text;
        QStringRef innerText;
    };
    
    class Node {
    public:
        Node(const QStringRef& text)
            : text_(text)
        {
        }
        
        const QStringRef& text() const {
            return text_;
        }
        
        virtual QString render(const Context& context) const = 0;

    private:
        const QStringRef text_;
    };

    class TextNode : public Node {
    public:
        TextNode(const QStringRef& text)
            : Node(text)
        {
        }
        
        virtual QString render(const Context&) const {
            return text().toString();
        }
    };
    
    class ObjectNode : public Node {
    public:
        ObjectNode(const QStringRef& text, const Variable& var)
            : Node(text)
            , var_(var)
        {
        }

        virtual QString render(const Context& context) const {
            return var_.evaluate(context).toString();
        }
    private:
        const Variable var_;
    };

    class TagNode : public Node {
    public:
        TagNode(const QStringRef& text)
            : Node(text)
        {
        }
        
        virtual QString render(const Context&) const {
            return "";
        }
    };
    
    using NodePtr = std::shared_ptr<Node>;
}

#endif
