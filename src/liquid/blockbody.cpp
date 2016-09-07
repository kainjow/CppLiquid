#include "blockbody.hpp"
#include "tokenizer.hpp"
#include "assign.hpp"
#include "comment.hpp"
#include <QDebug>

void Liquid::BlockBody::defaultUnknownTagHandler(const QStringRef& tagName, Tokenizer&)
{
    if (!tagName.isNull()) {
        throw QString("Unknown tag '%1'").arg(tagName.toString()).toStdString();
    }
}

void Liquid::BlockBody::parse(Tokenizer& tokenizer, const UnknownTagHandler unknownTagHandler) {
    nodes_.clear();
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
                } else if (tagName == "comment") {
                    std::shared_ptr<CommentTag> comment = std::make_shared<CommentTag>(tagName);
                    comment->parse(tokenizer);
                    nodes_.push_back(comment);
                } else {
                    unknownTagHandler(tagName, tokenizer);
                    return;
                }
                break;
            }
        }
    }
    unknownTagHandler(QStringRef(), tokenizer);
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
