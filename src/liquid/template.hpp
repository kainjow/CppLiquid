#ifndef LIQUID_TEMPLATE_HPP
#define LIQUID_TEMPLATE_HPP

#include <QString>
#include <vector>

namespace Liquid {

    class Template {
    public:
        bool parse(const QString& source, QString& errorMessage);
        
        QString render();
        
    private:
        class Component {
        public:
            enum class Type {
                Text,
                Object,
                Tag,
            };
            Component(const Type& type, const QStringRef text)
                : type_(type)
                , text_(text)
            {
            }
            Type type() const {
                return type_;
            }
            const QStringRef text() const {
                return text_;
            }
        private:
            const Type type_;
            const QStringRef text_;
        };
        
        std::vector<Component> components_;
        QString source_;
    };

}

#endif
