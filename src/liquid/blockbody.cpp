#include "blockbody.hpp"
#include "tokenizer.hpp"
#include "node.hpp"
#include <QDebug>

Liquid::BlockBody::BlockBody()
{
}

Liquid::BlockBody::BlockBody(Tokenizer& tokenizer) {
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
                if (tagName == "assign") {
                    nodes_.push_back(std::make_shared<AssignTag>(parser));
                } else {
                    throw QString("Unknown tag '%1'").arg(tagName.toString()).toStdString();
                }
                break;
            }
        }
    }
}

QString Liquid::BlockBody::render(Context& context) {
    QString str;
    for (const auto& node : nodes_) {
        str += node->render(context);
    }
    return str;
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::BlockBody") {
    
}

#endif
