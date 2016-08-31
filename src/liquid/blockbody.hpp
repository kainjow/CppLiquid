#ifndef LIQUID_BLOCKBODY_HPP
#define LIQUID_BLOCKBODY_HPP

#include "tokenizer.hpp"
#include "context.hpp"
#include "node.hpp"
#include "parser.hpp"

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
                        nodes_.push_back(std::make_shared<TextNode>(comp->text));
                        break;
                    case Component::Type::Object:
                        nodes_.push_back(std::make_shared<ObjectNode>(Variable(comp->innerText)));
                        break;
                    case Component::Type::Tag: {
                        Parser parser(comp->innerText);
                        const QStringRef tagName = parser.consume(Token::Type::Id);
                        if (tagName == "raw") {
                            std::vector<QStringRef> refs;
                            bool gotEnd = false;
                            while ((comp = tokenizer.next()) != nullptr) {
                                if (comp->type == Component::Type::Tag && comp->innerText == "endraw") {
                                    gotEnd = true;
                                    break;
                                }
                                refs.push_back(comp->text);
                            }
                            if (!gotEnd) {
                                throw std::string("endraw tag not found for raw");
                            }
                            nodes_.push_back(std::make_shared<RawTag>(refs, tagName, parser));
                        } else {
                            nodes_.push_back(std::make_shared<TagNode>(tagName, parser));
                        }
                        break;
                    }
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
