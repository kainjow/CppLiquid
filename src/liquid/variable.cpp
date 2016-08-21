#include "variable.hpp"
#include <QDebug>

const Liquid::Context& Liquid::Variable::evaluate(const Context& context) const
{
    return exp_.evaluate(context);
}



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Variable") {
    

}

#endif
