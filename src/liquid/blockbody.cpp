#include "blockbody.hpp"
#include "tokenizer.hpp"
#include "stringscanner.hpp"
#include "assign.hpp"
#include "capture.hpp"
#include "case.hpp"
#include "comment.hpp"
#include "cycle.hpp"
#include "decrement.hpp"
#include "for.hpp"
#include "increment.hpp"
#include <QDebug>

void Liquid::BlockBody::defaultUnknownTagHandler(const QStringRef& tagName, const QStringRef&, Tokenizer&)
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
                } else if (tagName == "cycle") {
                    nodes_.push_back(std::make_shared<CycleTag>(tagName, markup));
                } else if (tagName == "case") {
                    std::shared_ptr<CaseTag> tag = std::make_shared<CaseTag>(tagName, markup);
                    tag->parse(tokenizer);
                    nodes_.push_back(tag);
                } else if (tagName == "for") {
                    std::shared_ptr<ForTag> tag = std::make_shared<ForTag>(tagName, markup);
                    tag->parse(tokenizer);
                    nodes_.push_back(tag);
                } else {
                    unknownTagHandler(tagName, markup, tokenizer);
                    return;
                }
                break;
            }
        }
    }
    unknownTagHandler(QStringRef(), QStringRef(), tokenizer);
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
