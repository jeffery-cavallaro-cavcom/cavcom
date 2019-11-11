// Creates and sets a percentage value.

#include <libunittest/all.hpp>

#include "percent.h"

using namespace cavcom::utility;

static const std::string NAME = "name";
static const double VALUE = 0.123456789;

TEST(create_percent_value) {
  Percent percent(NAME);
  percent.value(VALUE);
  UNITTEST_ASSERT_EQUAL(percent.value(), 12.3);
}
