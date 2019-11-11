// Creates and increments a counter.

#include <string>

#include <libunittest/all.hpp>

#include "counter.h"

using namespace cavcom::utility;

static const std::string NAME = "counter";

TEST(add_to_counter) {
  Counter counter(NAME);
  UNITTEST_ASSERT_EQUAL(counter.name(), NAME);
  UNITTEST_ASSERT_EQUAL(counter.value(), 0);

  counter.add();
  UNITTEST_ASSERT_EQUAL(counter.value(), 1);

  counter.add();
  counter.add();
  UNITTEST_ASSERT_EQUAL(counter.value(), 3);

  counter.add(100);
  UNITTEST_ASSERT_EQUAL(counter.value(), 103);

  counter.reset();
  UNITTEST_ASSERT_EQUAL(counter.value(), 0);
}
