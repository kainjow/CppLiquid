#ifndef LIQUID_COMMENT_HPP
#define LIQUID_COMMENT_HPP

#include "block.hpp"

namespace Liquid {
    
    class CommentTag : public BlockTag {
    public:
        CommentTag(const Context& context, const StringRef& tagName, const StringRef& markup)
            : BlockTag(context, tagName, markup)
        {
        }
        
        virtual String render(Context&) override {
            return "";
        }

    protected:
        virtual void handleUnknownTag(const StringRef&, const StringRef&, Tokenizer&) override {
            // ignore
        }
    };
}

#endif
