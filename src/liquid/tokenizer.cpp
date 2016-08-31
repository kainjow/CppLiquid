#include "tokenizer.hpp"
#include "stringscanner.hpp"
#include <QDebug>

std::vector<Liquid::Component> Liquid::Tokenizer::tokenize(const QString& source) const
{
    std::vector<Component> components;
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
                components.emplace_back(Component::Type::Text, text, text);
            }
            
            // Collect the complete text of the object or tag
            const int tagEndPos = endPos + 2;
            const QStringRef tag = source.midRef(startPos, tagEndPos - startPos);
            const QStringRef tagTrimmed = tag.mid(2, tag.size() - 4).trimmed();
            
            lastStartPos = tagEndPos;
            
            // Process special tags
            bool addComponent = true;
            if (!isObject) {
                if (tagTrimmed == "raw") {
                    // We have a {% raw %} tag. Scan for the complete {% endraw %} tag
                    StringScanner ss(&source, lastStartPos);
                    bool foundRawEnd = false;
                    int rawendPos = -1;
                    const QString tagStartStr = "{%";
                    while (!ss.eof()) {
                        const int savePos = ss.position();
                        if (ss.scanUpTo(tagStartStr)) {
                            rawendPos = ss.position();
                            ss.advance(tagStartStr.size());
                            (void)ss.skipWhitespace();
                            const QStringRef tagIdentifier = ss.scanIdentifier();
                            if (tagIdentifier == "endraw") {
                                (void)ss.skipWhitespace();
                                if (ss.scanString(endStr[1])) {
                                    foundRawEnd = true;
                                    break;
                                }
                            }
                        }
                        ss.setPosition(savePos + 1);
                    }
                    if (!foundRawEnd) {
                        throw std::string("raw tag now properly terminated");
                    }
                    const int chunkLen = rawendPos - lastStartPos;
                    if (chunkLen > 0) {
                        const QStringRef text = source.midRef(lastStartPos, chunkLen);
                        components.emplace_back(Component::Type::Text, text, text);
                    }
                    addComponent = false;
                    lastStartPos = ss.position();
                }
            }
            
            if (addComponent) {
                components.emplace_back(isObject ? Component::Type::Object : Component::Type::Tag, tag, tagTrimmed);
            }
        }
    }
    
    // Process any remaining text
    if (lastStartPos < len) {
        const QStringRef text = source.midRef(lastStartPos);
        components.emplace_back(Component::Type::Text, text, text);
    }
    
    return components;
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Tokenizer") {
    
}

#endif
