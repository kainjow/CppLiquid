#ifndef LIQUID_COMPONENT_HPP
#define LIQUID_COMPONENT_HPP

#include <QStringRef>

namespace Liquid {

    class Component {
    public:
        enum class Type {
            Text,
            Object,
            Tag,
        };
        
        Component(const Type& type, const QStringRef& text)
            : type_(type)
            , text_(text)
        {
        }
        
        Type type() const {
            return type_;
        }
        
        const QStringRef& text() const {
            return text_;
        }

    private:
        const Type type_;
        const QStringRef text_;
    };

}

#endif
