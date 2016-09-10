#include "catch.hpp"

#define CHECK_TEMPLATE_RESULT(i,o) CHECK(Liquid::Template().parse(i).render().toStdString() == o)
