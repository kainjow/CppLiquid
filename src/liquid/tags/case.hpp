#ifndef LIQUID_CASE_HPP
#define LIQUID_CASE_HPP

#include "block.hpp"
#include "expression.hpp"

namespace Liquid {
    
    class CaseTag : public BlockTag {
    public:
        CaseTag(const QStringRef& tagName, const QStringRef& markup);
        
        virtual void parse(Tokenizer& tokenizer) override;
        
        virtual QString render(Context& context) override;
        
    protected:
        virtual void handleUnknownTag(const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer) override;
        
    private:
        Expression left_;
        
        class CaseCondition {
        public:
            explicit CaseCondition(bool els, const BlockBody& block)
                : else_(els)
                , block_(block)
            {
            }
            explicit CaseCondition(const Expression& expression, const BlockBody& block)
                : else_(false)
                , expression_(expression)
                , block_(block)
            {
            }
            bool isElse() const {
                return else_;
            }
            const Expression& expression() const {
                return expression_;
            }
            const BlockBody& block() const {
                return block_;
            }
            BlockBody& block() {
                return block_;
            }
        private:
            bool else_;
            Expression expression_;
            BlockBody block_;
        };
        
        std::vector<CaseCondition> conditions_;
    };
}

#endif
