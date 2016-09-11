#ifndef LIQUID_FOR_HPP
#define LIQUID_FOR_HPP

#include "block.hpp"
#include "expression.hpp"

namespace Liquid {
    
    class ForTag : public BlockTag {
    public:
        ForTag(const QStringRef& tagName, const QStringRef& markup);
        
        virtual void parse(Tokenizer& tokenizer) override;
        
        virtual QString render(Context& context) override;
        
    protected:
        virtual void handleUnknownTag(const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer) override;
        
    private:
        QStringRef varName_;
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
