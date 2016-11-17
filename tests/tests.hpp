#include "catch.hpp"

#define CHECK_TEMPLATE_RESULT(i,o) { \
    Liquid::Template __t; \
    __t.parse(i); \
    CHECK(__t.render() == o); \
}

#define CHECK_TEMPLATE_DATA_RESULT(i,o,d) { \
    Liquid::Data __d{d}; \
    Liquid::Template __t; \
    __t.parse(i); \
    CHECK(__t.render(__d) == o); \
}

#define CHECK_DATA_RESULT(t,o,d) { \
    Liquid::Data __d{d}; \
    CHECK(t.render(__d) == o); \
}
