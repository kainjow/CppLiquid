#ifndef LIQUID_BLOCKBODY_HPP
#define LIQUID_BLOCKBODY_HPP

#include "tokenizer.hpp"
#include "context.hpp"

namespace Liquid {

    class BlockBody {
    public:
        BlockBody()
        {
        }
        
        BlockBody(Tokenizer& tokenizer) {
            Tokenizer::ComponentPtr comp;
            while ((comp = tokenizer.next()) != nullptr) {
                components_.push_back(comp);
            }
        }
        
        QString render(const Context& context) {
            QString str;
            for (const auto& component : components_) {
                str += component->render(context);
            }
            return str;
        }

    private:
        std::vector<Tokenizer::ComponentPtr> components_;
    };

}

#endif
