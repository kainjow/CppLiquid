#ifndef LIQUID_TOKENIZER_HPP
#define LIQUID_TOKENIZER_HPP

#include <QStringRef>
#include <vector>

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
    
    class Tokenizer {
    public:
        Tokenizer(const QString& source)
            : tokens_(tokenize(source))
            , pos_(0)
        {
        }
        
        const Component* next() {
            if (pos_ >= tokens_.size()) {
                return nullptr;
            }
            const Component *comp = &tokens_.at(pos_);
            ++pos_;
            return comp;
        }
        
    private:
        const std::vector<Component> tokens_;
        size_t pos_;

        std::vector<Component> tokenize(const QString& source) const;
    };

}

#endif
