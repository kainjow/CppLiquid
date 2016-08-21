#include "variable.hpp"
#include <QDebug>

Liquid::Variable::Variable(const QStringRef& input)
{
    Parser parser(input);
    exp_ = Expression::parse(parser);
    
    while (parser.look(Token::Type::Pipe)) {
        (void)parser.consume();
        const QStringRef name = parser.consume(Token::Type::Id);
        std::vector<Expression> args;
        if (parser.look(Token::Type::Colon)) {
            (void)parser.consume();
            for (;;) {
                args.push_back(Expression::parse(parser));
                if (!parser.look(Token::Type::Comma)) {
                    break;
                }
                (void)parser.consume();
            }
        }
        filters_.emplace_back(name, args);
    }
}

const Liquid::Context& Liquid::Variable::evaluate(const Context& context) const
{
    const Context& result = exp_.evaluate(context);
    if (filters_.empty()) {
        return result;
    }
    Context value = result;
    for (const auto& filter : filters_) {
        const auto& args = filter.args();
        if (filter.name() == "append") {
            if (!value.isString()) {
                throw QString("append argument must be a string.").toStdString();
            }
            if (args.size() != 1) {
                throw QString("Expected 1 arg, but got %1").arg(args.size()).toStdString();
            }
            value = value.toString() + args[0].toString();
        } else {
            throw QString("Unknown filter %1").arg(filter.name().toString()).toStdString();
        }
    }
    cached_ = value;
    return cached_;
}



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Variable") {
    

}

#endif
