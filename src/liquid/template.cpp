#include "template.hpp"
#include <QDebug>

#define ABORT(msg) errorMessage = (msg); return false;

bool Liquid::Template::parse(const QString& source, QString& errorMessage)
{
    source_ = source;
    
    int lastStartPos = 0;
    const int len = source_.size();
    const int lastCharPos = len - 1;
    const QString endStr[] = {"}}", "%}"};
    components_.clear();
    
    while (lastStartPos < len) {
        // Look for the next starting object or tag
        const int startPos = source_.indexOf('{', lastStartPos);
        if (startPos == -1 || startPos == lastCharPos) {
            break;
        }
        
        // Check the next character
        const QChar nextChar = source_.at(startPos + 1);
        if (nextChar == '{' || nextChar == '%') {
            // Look for the end of the object or tag
            const bool isObject = nextChar == '{';
            const int endPos = source_.indexOf(endStr[isObject ? 0 : 1], startPos + 2);
            if (endPos == -1) {
                ABORT("Tag not properly terminated");
            }
            
            // Collect the complete text of the object or tag
            const int tagEndPos = endPos + 2;
            const QStringRef tag = source_.midRef(startPos, tagEndPos - startPos);
            components_.emplace_back(isObject ? Component::Type::Object : Component::Type::Tag, tag);
            
            // Collect any text component before the object or tag
            const int chunkLen = startPos - lastStartPos;
            if (chunkLen > 0) {
                const QStringRef text = source_.midRef(lastStartPos, chunkLen);
                components_.emplace_back(Component::Type::Text, text);
            }
            
            lastStartPos = tagEndPos;
        }
    }
    
    // Process any remaining text
    if (lastStartPos < len) {
        const QStringRef text = source_.midRef(lastStartPos);
        components_.emplace_back(Component::Type::Text, text);
    }
    
    return true;
}

QString Liquid::Template::render()
{
    QString str;
    for (const auto& component : components_) {
        switch (component.type()) {
            case Component::Type::Text:
                str += component.text();
                break;

            case Component::Type::Object:
                break;

            case Component::Type::Tag:
                break;
        }
    }
    return str;
}



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Template") {
    
    SECTION("NoParse") {
        Liquid::Template t;
        CHECK(t.render() == "");
    }

    SECTION("Empty") {
        Liquid::Template t;
        QString errmsg;
        CHECK(t.parse("", errmsg));
        CHECK(errmsg == "");
        CHECK(t.render() == "");
    }

    SECTION("PlainText") {
        Liquid::Template t;
        QString errmsg;
        CHECK(t.parse("Hello World!", errmsg));
        CHECK(errmsg == "");
        CHECK(t.render() == "Hello World!");
    }

    SECTION("SingleObject") {
        Liquid::Template t;
        QString errmsg;
        CHECK(t.parse("{{what}}", errmsg));
        CHECK(errmsg == "");
        CHECK(t.render() == "");
    }
    
    SECTION("SingleObjectWithText") {
        Liquid::Template t;
        QString errmsg;
        CHECK(t.parse("Hello {{what}}!", errmsg));
        CHECK(errmsg == "");
        CHECK(t.render() == "Hello !");
    }

    SECTION("UnclosedObject") {
        Liquid::Template t;
        QString errmsg;
        CHECK_FALSE(t.parse("{{what", errmsg));
        CHECK(errmsg == "Tag not properly terminated");
    }

}

#endif
