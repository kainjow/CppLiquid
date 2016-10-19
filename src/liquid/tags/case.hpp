#ifndef LIQUID_CASE_HPP
#define LIQUID_CASE_HPP

#include "block.hpp"
#include "expression.hpp"

namespace Liquid {
    
    class CaseTag : public BlockTag {
    public:
        CaseTag(const Context& context, const StringRef& tagName, const StringRef& markup);
        
        virtual void parse(const Context& context, Tokenizer& tokenizer) override;
        
        virtual String render(Context& context) override;
        
    protected:
        virtual void handleUnknownTag(const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer) override;
        
    private:
        Expression left_;
        
        class CaseCondition {
        public:
            explicit CaseCondition(bool els, const BlockBody& block)
                : else_(els)
                , block_(block)
            {
            }
            explicit CaseCondition(const std::vector<Expression>& expressions, const BlockBody& block)
                : else_(false)
                , expressions_(expressions)
                , block_(block)
            {
            }
            bool isElse() const {
                return else_;
            }
            const std::vector<Expression>& expressions() const {
                return expressions_;
            }
            const BlockBody& block() const {
                return block_;
            }
            BlockBody& block() {
                return block_;
            }
        private:
            bool else_;
            std::vector<Expression> expressions_;
            BlockBody block_;
        };
        
        std::vector<CaseCondition> conditions_;
    };
}

#endif
