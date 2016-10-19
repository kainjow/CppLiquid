#include "blockbody.hpp"
#include "tokenizer.hpp"
#include "stringscanner.hpp"
#include "context.hpp"

void Liquid::BlockBody::defaultUnknownTagHandler(const StringRef& tagName, const StringRef&, Tokenizer&)
{
    if (!tagName.isEmpty()) {
        throw String("Unknown tag '%1'").arg(tagName.toString()).toStdString();
    }
}

void Liquid::BlockBody::parse(const Context& context, Tokenizer& tokenizer, const UnknownTagHandler unknownTagHandler) {
    nodes_.clear();
    const Component *comp = nullptr;
    const auto& tags = context.tags();
    while ((comp = tokenizer.next()) != nullptr) {
        switch (comp->type) {
            case Component::Type::Text:
                nodes_.push_back(std::make_shared<TextNode>(context, comp->text));
                break;
            case Component::Type::Object:
                nodes_.push_back(std::make_shared<ObjectNode>(context, Variable(comp->innerText)));
                break;
            case Component::Type::Tag: {
                StringScanner ss(comp->innerText);
                const StringRef tagName = ss.scanIdentifier();
                const StringRef markup = comp->innerText.mid(ss.position());
                const auto tag = tags.find(tagName.toString());
                if (tag != tags.end()) {
                    nodes_.push_back(tag->second(context, tagName, markup, tokenizer));
                } else {
                    unknownTagHandler(tagName, markup, tokenizer);
                    return;
                }
                break;
            }
        }
    }
    unknownTagHandler(StringRef(), StringRef(), tokenizer);
}

Liquid::String Liquid::BlockBody::render(Context& context) {
    String str;
    for (const auto& node : nodes_) {
        str += node->render(context);
        if (context.haveInterrupt()) {
            break;
        }
    }
    return str;
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::BlockBody") {
    
}

#endif
