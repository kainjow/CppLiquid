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
            const Component *comp = nullptr;
            while ((comp = tokenizer.next()) != nullptr) {
                switch (comp->type) {
                    case Component::Type::Text:
                        nodes_.push_back(std::make_unique<TextNode>(comp->text));
                        break;
                    case Component::Type::Object:
                        nodes_.push_back(std::make_unique<ObjectNode>(comp->text, Variable(comp->innerText)));
                        break;
                    case Component::Type::Tag:
                        nodes_.push_back(std::make_unique<TagNode>(comp->text));
                        break;
                }
            }
        }
        
        QString render(const Context& context) {
            QString str;
            for (const auto& node : nodes_) {
                str += node->render(context);
            }
            return str;
        }

    private:
        std::vector<NodePtr> nodes_;
    };

}

#endif
