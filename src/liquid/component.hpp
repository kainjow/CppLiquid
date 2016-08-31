#ifndef LIQUID_COMPONENT_HPP
#define LIQUID_COMPONENT_HPP

#include "context.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "variable.hpp"

namespace Liquid {
    
    class Node {
    public:
        virtual QString render(const Context& context) const = 0;
    };

    class TextNode : public Node {
    public:
        TextNode(const QStringRef& text)
            : text_(text)
        {
        }
        
        virtual QString render(const Context&) const {
            return text_.toString();
        }
        
    private:
        const QStringRef text_;
    };
    
    class ObjectNode : public Node {
    public:
        ObjectNode(const Variable& var)
            : var_(var)
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
        TagNode(const QStringRef& tagName, Parser& parser)
            : tagName_(tagName)
        {
            parse(parser);
        }
        
        virtual QString render(const Context&) const {
            return "";
        }
                  
        virtual void parse(Parser& /*parser*/) {
            
        }
        
    private:
        QStringRef tagName_;
    };
    
    class BlockTag : public TagNode {
    public:
        BlockTag(const QStringRef& tagName, Parser& parser) : TagNode(tagName, parser) {}
    };
    
    class RawTag : public TagNode {
    public:
        RawTag(const std::vector<QStringRef>& refs, const QStringRef& tagName, Parser& parser)
            : TagNode(tagName, parser)
            , refs_(refs)
        {}
        
        virtual QString render(const Context&) const {
            QString str;
            for (const auto& ref : refs_) {
                str += ref.toString();
            }
            return str;
        }
    private:
        std::vector<QStringRef> refs_;
    };
    
    using NodePtr = std::shared_ptr<Node>;
}

#endif
