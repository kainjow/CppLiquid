#include "catch.hpp"

#define CHECK_TEMPLATE_RESULT(i,o) \
    CHECK(Liquid::Template{}.parse(i).render().toStdString() == o)

#define CHECK_TEMPLATE_DATA_RESULT(i,o,d) { \
    Liquid::Data data{d}; \
    CHECK(Liquid::Template{}.parse(i).render(data).toStdString() == o); \
}

#define CHECK_DATA_RESULT(t,o,d) { \
    Liquid::Data data{d}; \
    CHECK(t.render(data).toStdString() == o); \
}
