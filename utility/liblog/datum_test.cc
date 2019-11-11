// Creates and sets a datum.

#include <libunittest/all.hpp>

#include "datum.h"

using namespace cavcom::utility;

static const std::string NAME = "name";
static const uint VALUE = 42;

TEST(set_value) {
  Datum<uint> datum(NAME);
  UNITTEST_ASSERT_EQUAL(datum.name(), NAME);
  UNITTEST_ASSERT_EQUAL(datum.value(), 0);

  datum.value(VALUE);
  UNITTEST_ASSERT_EQUAL(datum.name(), NAME);
  UNITTEST_ASSERT_EQUAL(datum.value(), VALUE);
}

TEST(reset_value) {
  Datum<uint> datum(NAME);
  datum.value(VALUE);
  UNITTEST_ASSERT_EQUAL(datum.name(), NAME);
  UNITTEST_ASSERT_EQUAL(datum.value(), VALUE);

  datum.reset();
  UNITTEST_ASSERT_EQUAL(datum.name(), NAME);
  UNITTEST_ASSERT_EQUAL(datum.value(), 0);
}
