#ifndef LIQUID_BLOCK_HPP
#define LIQUID_BLOCK_HPP

#include "node.hpp"
#include "blockbody.hpp"

namespace Liquid {
    
    class BlockTag : public TagNode {
    public:
        BlockTag(const QStringRef& tagName, const QStringRef& markup);
        void parse(Tokenizer& tokenizer);
        
        bool parseBody(BlockBody& body, Tokenizer& tokenizer);
        
        virtual QString render(Context& context) override;
        
    protected:
        virtual void handleUnknownTag(const QStringRef& tagName, Tokenizer& tokenizer);

    private:
        QStringRef tagName_;
        BlockBody body_;
    };
}

#endif
