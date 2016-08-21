#ifndef LIQUID_COMPONENT_HPP
#define LIQUID_COMPONENT_HPP

#include "expression.hpp"
#include "context.hpp"

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
        
        virtual QString render(const Context& context) const {
            return text().toString();
        }
    };
    
    class ObjectComponent : public Component {
    public:
        ObjectComponent(const QStringRef& text, const Expression& expression)
            : Component(text)
            , exp_(expression)
        {
        }

        const Expression& expression() const {
            return exp_;
        }
        
        virtual QString render(const Context& context) const {
            return exp_.evaluate(context).toString();
        }
    private:
        const Expression exp_;
    };

    class TagComponent : public Component {
    public:
        TagComponent(const QStringRef& text)
            : Component(text)
        {
        }
        
        virtual QString render(const Context& context) const {
            return "";
        }
    };
}

#endif
