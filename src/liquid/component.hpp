#ifndef LIQUID_COMPONENT_HPP
#define LIQUID_COMPONENT_HPP

#include "context.hpp"
#include "variable.hpp"

namespace Liquid {
    
    class Component {
    public:
        Component(const QStringRef& text)
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

    class TextComponent : public Component {
    public:
        TextComponent(const QStringRef& text)
            : Component(text)
        {
        }
        
        virtual QString render(const Context&) const {
            return text().toString();
        }
    };
    
    class ObjectComponent : public Component {
    public:
        ObjectComponent(const QStringRef& text, const Variable& var)
            : Component(text)
            , var_(var)
        {
        }

        virtual QString render(const Context& context) const {
            return var_.evaluate(context).toString();
        }
    private:
        const Variable var_;
    };

    class TagComponent : public Component {
    public:
        TagComponent(const QStringRef& text)
            : Component(text)
        {
        }
        
        virtual QString render(const Context&) const {
            return "";
        }
    };
}

#endif
