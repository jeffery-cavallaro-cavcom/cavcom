// Generates Mycielski graphs.

#include <libunittest/all.hpp>

#include "mycielski.h"

#include "bron2.h"
#include "quick_zykov.h"

using namespace cavcom::graph;

TEST(null_graph) {
  Mycielski mg(0);
  UNITTEST_ASSERT_TRUE(mg.null());
}

TEST(trivial_graph) {
  Mycielski mg(1);
  UNITTEST_ASSERT_TRUE(mg.empty());
  UNITTEST_ASSERT_EQUAL(mg.order(), 1);
}

TEST(path_graph) {
  Mycielski mg(2);
  UNITTEST_ASSERT_EQUAL(mg.order(), 2);
  UNITTEST_ASSERT_TRUE(mg.adjacent(0, 1));
}

TEST(cycle_graph) {
  Mycielski mg(3);
  UNITTEST_ASSERT_EQUAL(mg.order(), 5);
  Bron2 b(mg, Bron::MODE_FIRST_MAX, false);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.number(), 2);
  QuickZykov qz(mg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.number(), 3);
}

TEST(grotsch_graph) {
  Mycielski mg(4);
  Bron2 b(mg, Bron::MODE_FIRST_MAX, false);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.number(), 2);
  QuickZykov qz(mg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.number(), 4);
}
