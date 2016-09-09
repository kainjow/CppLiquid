#include "blockbody.hpp"
#include "tokenizer.hpp"
#include "stringscanner.hpp"
#include "assign.hpp"
#include "capture.hpp"
#include "comment.hpp"
#include "decrement.hpp"
#include "increment.hpp"
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
                StringScanner ss(comp->innerText);
                const QStringRef tagName = ss.scanIdentifier();
                const QStringRef markup = comp->innerText.mid(ss.position());
                if (tagName == "assign") {
                    nodes_.push_back(std::make_shared<AssignTag>(tagName, markup));
                } else if (tagName == "comment") {
                    std::shared_ptr<CommentTag> tag = std::make_shared<CommentTag>(tagName, markup);
                    tag->parse(tokenizer);
                    nodes_.push_back(tag);
                } else if (tagName == "capture") {
                    std::shared_ptr<CaptureTag> tag = std::make_shared<CaptureTag>(tagName, markup);
                    tag->parse(tokenizer);
                    nodes_.push_back(tag);
                } else if (tagName == "increment") {
                    nodes_.push_back(std::make_shared<IncrementTag>(tagName, markup));
                } else if (tagName == "decrement") {
                    nodes_.push_back(std::make_shared<DecrementTag>(tagName, markup));
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
