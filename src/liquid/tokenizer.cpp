#include "tokenizer.hpp"
#include <QDebug>

std::vector<Liquid::Tokenizer::ComponentPtr> Liquid::Tokenizer::tokenize(const QString& source)
{
    std::vector<ComponentPtr> components;
    int lastStartPos = 0;
    const int len = source.size();
    const int lastCharPos = len - 1;
    const QString endStr[] = {"}}", "%}"};
    
    while (lastStartPos < len) {
        // Look for the next starting object or tag
        const int startPos = source.indexOf('{', lastStartPos);
        if (startPos == -1 || startPos == lastCharPos) {
            break;
        }
        
        // Check the next character
        const QChar nextChar = source.at(startPos + 1);
        if (nextChar == '{' || nextChar == '%') {
            // Look for the end of the object or tag
            const bool isObject = nextChar == '{';
            const int endPos = source.indexOf(endStr[isObject ? 0 : 1], startPos + 2);
            if (endPos == -1) {
                throw std::string("Tag not properly terminated");
            }
            
            // Collect any text component before the object or tag
            const int chunkLen = startPos - lastStartPos;
            if (chunkLen > 0) {
                const QStringRef text = source.midRef(lastStartPos, chunkLen);
                components.push_back(std::make_unique<TextComponent>(text));
            }
            
            // Collect the complete text of the object or tag
            const int tagEndPos = endPos + 2;
            const QStringRef tag = source.midRef(startPos, tagEndPos - startPos);
            const QStringRef tagTrimmed = tag.mid(2, tag.size() - 4).trimmed();
            if (isObject) {
                components.push_back(std::make_unique<ObjectComponent>(tag, Variable(tagTrimmed)));
            } else {
                components.push_back(std::make_unique<TagComponent>(tag));
            }
            
            lastStartPos = tagEndPos;
        }
    }
    
    // Process any remaining text
    if (lastStartPos < len) {
        const QStringRef text = source.midRef(lastStartPos);
        components.push_back(std::make_unique<TextComponent>(text));
    }
    
    return components;
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Tokenizer") {
    
}

#endif
