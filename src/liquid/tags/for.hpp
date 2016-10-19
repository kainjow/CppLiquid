#ifndef LIQUID_FOR_HPP
#define LIQUID_FOR_HPP

#include "block.hpp"
#include "expression.hpp"

namespace Liquid {
    
    class ForTag : public BlockTag {
    public:
        ForTag(const Context& context, const StringRef& tagName, const StringRef& markup);
        
        virtual void parse(const Context& context, Tokenizer& tokenizer) override;
        
        virtual String render(Context& context) override;
        
    protected:
        virtual void handleUnknownTag(const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer) override;
        
    private:
        StringRef varName_;
        BlockBody elseBlock_;
        bool range_;
        Expression rangeStart_;
        Expression rangeEnd_;
        Expression collection_;
        bool reversed_;
        Expression offset_;
        Expression limit_;
    };
}

#endif
