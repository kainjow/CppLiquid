#ifndef LIQUID_TOKENIZER_HPP
#define LIQUID_TOKENIZER_HPP

#include "component.hpp"

namespace Liquid {

    class Tokenizer {
    public:
        using ComponentPtr = std::shared_ptr<Component>;
        
        Tokenizer(const QString& source)
            : tokens_(tokenize(source))
            , pos_(0)
        {
        }
        
        ComponentPtr next() {
            if (pos_ >= tokens_.size()) {
                return nullptr;
            }
            const ComponentPtr ptr = tokens_.at(pos_);
            ++pos_;
            return ptr;
        }
        
        const std::vector<ComponentPtr>& tokens() const {
            return tokens_;
        }

    private:
        std::vector<ComponentPtr> tokens_;
        size_t pos_;

        std::vector<ComponentPtr> tokenize(const QString& source) const;
    };

}

#endif
