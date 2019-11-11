// Creates and sets a sample.

#include <libunittest/all.hpp>

#include "sample.h"

using namespace cavcom::utility;

static const std::string NAME = "name";

TEST(set_values) {
  Sample<uint> sample(NAME);
  UNITTEST_ASSERT_EQUAL(sample.total(), 0);
  UNITTEST_ASSERT_EQUAL(sample.count(), 0);
  UNITTEST_ASSERT_EQUAL(sample.mean(), 0.0);
  UNITTEST_ASSERT_EQUAL(sample.min(), 0);
  UNITTEST_ASSERT_EQUAL(sample.max(), 0);

  sample.add(1);
  UNITTEST_ASSERT_EQUAL(sample.total(), 1);
  UNITTEST_ASSERT_EQUAL(sample.count(), 1);
  UNITTEST_ASSERT_EQUAL(sample.mean(), 1.0);
  UNITTEST_ASSERT_EQUAL(sample.min(), 1);
  UNITTEST_ASSERT_EQUAL(sample.max(), 1);

  sample.add(4);
  UNITTEST_ASSERT_EQUAL(sample.total(), 5);
  UNITTEST_ASSERT_EQUAL(sample.count(), 2);
  UNITTEST_ASSERT_EQUAL(sample.mean(), 2.5);
  UNITTEST_ASSERT_EQUAL(sample.min(), 1);
  UNITTEST_ASSERT_EQUAL(sample.max(), 4);

  sample.add(0);
  sample.add(0);
  UNITTEST_ASSERT_EQUAL(sample.total(), 5);
  UNITTEST_ASSERT_EQUAL(sample.count(), 4);
  UNITTEST_ASSERT_EQUAL(sample.mean(), 1.25);
  UNITTEST_ASSERT_EQUAL(sample.min(), 0);
  UNITTEST_ASSERT_EQUAL(sample.max(), 4);

  sample.add(5);
  UNITTEST_ASSERT_EQUAL(sample.total(), 10);
  UNITTEST_ASSERT_EQUAL(sample.count(), 5);
  UNITTEST_ASSERT_EQUAL(sample.mean(), 2);
  UNITTEST_ASSERT_EQUAL(sample.min(), 0);
  UNITTEST_ASSERT_EQUAL(sample.max(), 5);

  sample.reset();
  UNITTEST_ASSERT_EQUAL(sample.total(), 0);
  UNITTEST_ASSERT_EQUAL(sample.count(), 0);
  UNITTEST_ASSERT_EQUAL(sample.mean(), 0.0);
  UNITTEST_ASSERT_EQUAL(sample.min(), 0);
  UNITTEST_ASSERT_EQUAL(sample.max(), 0);
}
