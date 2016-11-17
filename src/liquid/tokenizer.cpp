#include "tokenizer.hpp"
#include "stringscanner.hpp"
#include "error.hpp"

std::vector<Liquid::Component> Liquid::Tokenizer::tokenize(const String& source) const
{
    std::vector<Component> components;
    String::size_type lastStartPos = 0;
    const String::size_type len = source.size();
    const String::size_type lastCharPos = len - 1;
    const String endStr[] = {"}}", "%}"};
    
    while (lastStartPos < len) {
        // Look for the next starting object or tag
        const String::size_type startPos = source.indexOf('{', lastStartPos);
        if (startPos == String::npos || startPos == lastCharPos) {
            break;
        }
        
        // Check the next character
        const String::value_type nextChar = source.at(startPos + 1);
        if (nextChar == '{' || nextChar == '%') {
            // Look for the end of the object or tag
            const bool isObject = nextChar == '{';
            const String::size_type endPos = source.indexOf(endStr[isObject ? 0 : 1], startPos + 2);
            if (endPos == String::npos) {
                throw syntax_error("Tag not properly terminated");
            }
            
            // Collect any text component before the object or tag
            const auto chunkLen = startPos - lastStartPos;
            if (chunkLen > 0) {
                const StringRef text = source.midRef(lastStartPos, chunkLen);
                components.emplace_back(Component::Type::Text, text, text);
            }
            
            // Collect the complete text of the object or tag
            const auto tagEndPos = endPos + 2;
            const StringRef tag = source.midRef(startPos, tagEndPos - startPos);
            const StringRef tagTrimmed = trim(tag.mid(2, tag.size() - 4));
            
            lastStartPos = tagEndPos;
            
            // Process special tags
            bool addComponent = true;
            if (!isObject) {
                const bool isRaw = tagTrimmed == "raw";
                if (isRaw) {
                    const String endTag = String("end") + tagTrimmed.toString();
                    // Scan for the complete {% endxxx %} tag
                    StringScanner ss(&source, lastStartPos);
                    bool foundEnd = false;
                    StringRef::size_type rawendPos = -1;
                    const String tagStartStr = "{%";
                    while (!ss.eof()) {
                        const auto savePos = ss.position();
                        if (ss.scanUpTo(tagStartStr)) {
                            rawendPos = ss.position();
                            ss.advance(static_cast<int>(tagStartStr.size()));
                            (void)ss.skipWhitespace();
                            const StringRef tagIdentifier = ss.scanIdentifier();
                            if (tagIdentifier == endTag) {
                                (void)ss.skipWhitespace();
                                if (ss.scanString(endStr[1])) {
                                    foundEnd = true;
                                    break;
                                }
                            }
                        }
                        ss.setPosition(savePos + 1);
                    }
                    if (!foundEnd) {
                        throw syntax_error(String("%1 tag not properly terminated").arg(tagTrimmed.toString()).toStdString());
                    }
                    if (isRaw) {
                        const auto chunkLen = rawendPos - lastStartPos;
                        if (chunkLen > 0) {
                            const StringRef text = source.midRef(lastStartPos, chunkLen);
                            components.emplace_back(Component::Type::Text, text, text);
                        }
                    }
                    addComponent = false;
                    lastStartPos = ss.position();
                }
            }
            
            if (addComponent) {
                components.emplace_back(isObject ? Component::Type::Object : Component::Type::Tag, tag, tagTrimmed);
            }
        } else {
            ++lastStartPos;
        }
    }
    
    // Process any remaining text
    if (lastStartPos < len) {
        const StringRef text = source.midRef(lastStartPos);
        components.emplace_back(Component::Type::Text, text, text);
    }
    
    return components;
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Tokenizer") {
    
}

#endif
