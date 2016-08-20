#include "template.hpp"
#include <QDebug>

#define ABORT(msg) throw QString(msg)

void Liquid::Template::parse(const QString& source)
{
    source_ = source;
    components_ = tokenize(source_);
}

std::vector<Liquid::Component> Liquid::Template::tokenize(const QString& source) const
{
    std::vector<Component> components;
    int lastStartPos = 0;
    const int len = source_.size();
    const int lastCharPos = len - 1;
    const QString endStr[] = {"}}", "%}"};
    
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
            components.emplace_back(isObject ? Component::Type::Object : Component::Type::Tag, tag);
            
            // Collect any text component before the object or tag
            const int chunkLen = startPos - lastStartPos;
            if (chunkLen > 0) {
                const QStringRef text = source_.midRef(lastStartPos, chunkLen);
                components.emplace_back(Component::Type::Text, text);
            }
            
            lastStartPos = tagEndPos;
        }
    }
    
    // Process any remaining text
    if (lastStartPos < len) {
        const QStringRef text = source_.midRef(lastStartPos);
        components.emplace_back(Component::Type::Text, text);
    }
    
    return components;
}

QString Liquid::Template::render()
{
    const Context ctx;
    return render(ctx);
}

QString Liquid::Template::render(const Context& ctx)
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

TEST_CASE("Liquid::Template") {
    
    SECTION("NoParse") {
        Liquid::Template t;
        CHECK(t.render() == "");
    }

    SECTION("Empty") {
        Liquid::Template t;
        t.parse("");
        CHECK(t.render() == "");
    }

    SECTION("PlainText") {
        Liquid::Template t;
        t.parse("Hello World!");
        CHECK(t.render() == "Hello World!");
    }

    SECTION("SingleObject") {
        Liquid::Template t;
        t.parse("{{what}}");
        CHECK(t.render() == "");
    }
    
    SECTION("SingleObjectWithText") {
        Liquid::Template t;
        t.parse("Hello {{what}}!");
        CHECK(t.render() == "Hello !");
    }

    SECTION("UnclosedObject") {
        Liquid::Template t;
        CHECK_THROWS_AS(t.parse("{{what"), QString);
    }
    
    SECTION("BasicObject") {
        Liquid::Template t;
        t.parse("Hello {{what}}");
        // TODO
    }

}

#endif
