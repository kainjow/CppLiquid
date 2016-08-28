#ifndef LIQUID_TOKENIZER_HPP
#define LIQUID_TOKENIZER_HPP

#include "component.hpp"

namespace Liquid {

    class Tokenizer {
    public:
        using ComponentPtr = std::unique_ptr<Component>;

        static std::vector<ComponentPtr> tokenize(const QString& source);
    };

}

#endif
