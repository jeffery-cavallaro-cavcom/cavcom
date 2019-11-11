// Creates and sets a datum.

#include <string>

#include <libunittest/all.hpp>

#include "datum.h"

using namespace cavcom::utility;

static const std::string NAME1 = "name1";
static const uint VALUE1 = 42;

static const std::string NAME2 = "name1";
static const double VALUE2 = -1.5;

TEST(create_datum_no_value) {
  Datum<uint> datum(NAME1);
  UNITTEST_ASSERT_EQUAL(datum.name(), NAME1);
  UNITTEST_ASSERT_EQUAL(datum.value(), 0);

  datum.value(VALUE1);
  UNITTEST_ASSERT_EQUAL(datum.value(), VALUE1);
}

TEST(create_datum_with_value) {
  Datum<double> datum(NAME2, VALUE2);
  UNITTEST_ASSERT_EQUAL(datum.name(), NAME2);
  UNITTEST_ASSERT_EQUAL(datum.value(), VALUE2);
}
