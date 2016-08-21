#ifndef LIQUID_COMPONENT_HPP
#define LIQUID_COMPONENT_HPP

#include "expression.hpp"

namespace Liquid {

    class Component {
    public:
        enum class Type {
            Text,
            Object,
            Tag,
        };
        
        Component(const Type& type)
            : type_(type)
        {
        }

        Component(const Type& type, const QStringRef& text)
            : type_(type)
            , text_(text)
        {
        }
        
        Component(const Expression& exp)
            : type_(Type::Object)
            , exp_(exp)
        {
        }
        
        Type type() const {
            return type_;
        }
        
        const QStringRef& text() const {
            return text_;
        }
        
        const Expression& expression() const {
            return exp_;
        }

    private:
        const Type type_;
        const QStringRef text_;
        const Expression exp_;
    };

}

#endif
