#ifndef LIQUID_COMMENT_HPP
#define LIQUID_COMMENT_HPP

#include "block.hpp"

namespace Liquid {
    
    class CommentTag : public BlockTag {
    public:
        CommentTag(const QStringRef& tagName) : BlockTag(tagName) {
        }
        
        virtual QString render(Context&) override {
            return "";
        }

    protected:
        virtual void handleUnknownTag(const QStringRef&, Tokenizer&) override {
            // ignore
        }
    };
}

#endif
