#ifndef LIQUID_TOKENIZER_HPP
#define LIQUID_TOKENIZER_HPP

#include "string.hpp"
#include <vector>

namespace Liquid {

    class Component {
    public:
        enum class Type {
            Text,
            Object,
            Tag,
        };

        Component(Type theType, const StringRef& theText, const StringRef& theInnerText)
            : type(theType)
            , text(theText)
            , innerText(theInnerText)
        {
        }
        
        Type type;
        StringRef text;
        StringRef innerText;
    };
    
    class Tokenizer {
    public:
        Tokenizer(const String& source)
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

        std::vector<Component> tokenize(const String& source) const;
    };

}

#endif
