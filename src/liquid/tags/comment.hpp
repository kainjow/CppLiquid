#ifndef LIQUID_COMMENT_HPP
#define LIQUID_COMMENT_HPP

#include "block.hpp"

namespace Liquid {
    
    class CommentTag : public BlockTag {
    public:
        CommentTag(const Context& context, const QStringRef& tagName, const QStringRef& markup)
            : BlockTag(context, tagName, markup)
        {
        }
        
        virtual QString render(Context&) override {
            return "";
        }

    protected:
        virtual void handleUnknownTag(const QStringRef&, const QStringRef&, Tokenizer&) override {
            // ignore
        }
    };
}

#endif
