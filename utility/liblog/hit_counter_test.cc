// Creates and sets a hit counter.

#include <libunittest/all.hpp>

#include "hit_counter.h"

using namespace cavcom::utility;

static const std::string NAME = "name";

TEST(set_values) {
  HitCounter hc(NAME);
  UNITTEST_ASSERT_EQUAL(hc.tries(), 0);
  UNITTEST_ASSERT_EQUAL(hc.hits(), 0);
  UNITTEST_ASSERT_EQUAL(hc.percent(), 0.0);

  hc.add(1, 1);
  UNITTEST_ASSERT_EQUAL(hc.tries(), 1);
  UNITTEST_ASSERT_EQUAL(hc.hits(), 1);
  UNITTEST_ASSERT_EQUAL(hc.percent(), 100.0);

  hc.add(1, 0);
  UNITTEST_ASSERT_EQUAL(hc.tries(), 2);
  UNITTEST_ASSERT_EQUAL(hc.hits(), 1);
  UNITTEST_ASSERT_EQUAL(hc.percent(), 50.0);

  hc.add(7, 2);
  UNITTEST_ASSERT_EQUAL(hc.tries(), 9);
  UNITTEST_ASSERT_EQUAL(hc.hits(), 3);
  UNITTEST_ASSERT_EQUAL(hc.percent(), 33.3);

  hc.reset();
  UNITTEST_ASSERT_EQUAL(hc.tries(), 0);
  UNITTEST_ASSERT_EQUAL(hc.hits(), 0);
  UNITTEST_ASSERT_EQUAL(hc.percent(), 0.0);
}

