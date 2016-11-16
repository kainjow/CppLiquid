#include "catch.hpp"

#define CHECK_TEMPLATE_RESULT(i,o) \
    CHECK(Liquid::Template{}.parse(i).render() == o)

#define CHECK_TEMPLATE_DATA_RESULT(i,o,d) { \
    Liquid::Data __liquid__tests__check_template_data__result__data__{d}; \
    CHECK(Liquid::Template{}.parse(i).render(__liquid__tests__check_template_data__result__data__) == o); \
}

#define CHECK_DATA_RESULT(t,o,d) { \
    Liquid::Data __liquid__tests__check_data_result__data__{d}; \
    CHECK(t.render(__liquid__tests__check_data_result__data__) == o); \
}
