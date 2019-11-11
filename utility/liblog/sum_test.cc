// Creates and increments a sum and a counter.

#include <libunittest/all.hpp>

#include "sum.h"

using namespace cavcom::utility;

static const std::string NAME = "counter";

TEST(add_to_sum) {
  Sum<double> sum(NAME);
  UNITTEST_ASSERT_EQUAL(sum.name(), NAME);
  UNITTEST_ASSERT_EQUAL(sum.value(), 0.0);

  sum.add(1.0);
  UNITTEST_ASSERT_EQUAL(sum.value(), 1.0);

  sum.add(-2.5);
  UNITTEST_ASSERT_EQUAL(sum.value(), -1.5);

  sum.add(12.25);
  UNITTEST_ASSERT_EQUAL(sum.value(), 10.75);

  sum.reset();
  UNITTEST_ASSERT_EQUAL(sum.value(), 0);
}

TEST(add_to_counter) {
  Counter counter(NAME);
  UNITTEST_ASSERT_EQUAL(counter.name(), NAME);
  UNITTEST_ASSERT_EQUAL(counter.value(), 0);

  counter.add(1);
  UNITTEST_ASSERT_EQUAL(counter.value(), 1);

  counter.add(1);
  counter.add(2);
  counter.add(3);
  UNITTEST_ASSERT_EQUAL(counter.value(), 7);

  counter.add(100);
  UNITTEST_ASSERT_EQUAL(counter.value(), 107);

  counter.reset();
  UNITTEST_ASSERT_EQUAL(counter.value(), 0);
}
