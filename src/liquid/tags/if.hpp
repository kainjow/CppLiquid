#ifndef LIQUID_IF_HPP
#define LIQUID_IF_HPP

#include "block.hpp"
#include "expression.hpp"

namespace Liquid {
    
    class Condition {
    public:
        enum class Operator {
            None,
            Equal,
            NotEqual,
            LessThan,
            GreaterThan,
            GreaterOrEqualThan,
            LessOrEqualThan,
            Contains,
        };
        
        Condition()
            : op_(Operator::None)
        {
        }
        
        Condition(const Expression& a)
            : a_(a)
            , op_(Operator::None)
        {
        }
        
        Condition(const Expression& a, Operator op, const Expression& b)
            : a_(a)
            , op_(op)
            , b_(b)
        {
            (void)op_;
        }
        
        bool evaluate(Context& context);
    private:
        Expression a_;
        Operator op_;
        Expression b_;
    };
    
    class IfBlock {
    public:
        BlockBody body;
        Condition cond;
    };
    
    class IfTag : public BlockTag {
    public:
        IfTag(const Context& context, const QStringRef& tagName, const QStringRef& markup);
        
        virtual void parse(const Context& context, Tokenizer& tokenizer) override;
        
        virtual QString render(Context& context) override;
        
    protected:
        virtual void handleUnknownTag(const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer) override;

    private:
        Condition parseCondition(Parser& parser);
        
        std::vector<IfBlock> blocks_;
    };
}

#endif
