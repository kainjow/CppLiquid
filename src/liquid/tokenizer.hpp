#ifndef LIQUID_TOKENIZER_HPP
#define LIQUID_TOKENIZER_HPP

#include "component.hpp"

namespace Liquid {

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
