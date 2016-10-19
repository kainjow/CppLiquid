#ifndef LIQUID_BLOCK_HPP
#define LIQUID_BLOCK_HPP

#include "node.hpp"
#include "blockbody.hpp"

namespace Liquid {
    
    class BlockTag : public TagNode {
    public:
        BlockTag(const Context& context, const StringRef& tagName, const StringRef& markup);
        
        virtual void parse(const Context& context, Tokenizer& tokenizer);
        
        bool parseBody(const Context& context, BlockBody* body, Tokenizer& tokenizer);
        
        virtual String render(Context& context) override;
        
    protected:
        virtual void handleUnknownTag(const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer);

        BlockBody body_;

    private:
        StringRef tagName_;
    };
}

#endif
