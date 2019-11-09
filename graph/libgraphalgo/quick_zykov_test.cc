// Tests the parts and whole of the quick Zykov algorithm.

#include <libunittest/all.hpp>

#include "quick_zykov.h"

using namespace cavcom::graph;

static const VertexValuesList VERTICES = {{"a", NOCOLOR, 2, 6},
                                          {"b", NOCOLOR, 0, 4},
                                          {"c", NOCOLOR, 4, 4},
                                          {"d", NOCOLOR, 0, 2},
                                          {"e", NOCOLOR, 4, 2},
                                          {"f", NOCOLOR, 2, 0},
                                          {"g", NOCOLOR, 0, 0},
                                          {"h", NOCOLOR, 4, 0}};

static const EdgeValuesList EDGES = {{0, 1}, {0, 2}, {0, 6},
                                     {1, 2}, {1, 3},
                                     {2, 4},
                                     {3, 4}, {3, 5}, {3, 6},
                                     {4, 5},
                                     {5, 6}, {5, 7}};
                                     

TEST(check_for_null) {
  // A null graph.
  QuickZykov::GraphPtr pg(new Graph);
  QuickZykov qznull(*pg);
  UNITTEST_ASSERT_EQUAL(qznull.steps(), 0);
  UNITTEST_ASSERT_TRUE(qznull.check_for_null(pg));
  UNITTEST_ASSERT_EQUAL(qznull.steps(), 1);

  // An empty graph.
  pg.reset(new Graph(5));
  QuickZykov qzempty(*pg);
  UNITTEST_ASSERT_EQUAL(qzempty.steps(), 0);
  UNITTEST_ASSERT_FALSE(qzempty.check_for_null(pg));
  UNITTEST_ASSERT_EQUAL(qzempty.steps(), 1);

  // A full graph.
  pg.reset(new Graph(VERTICES, EDGES));
  QuickZykov qzfull(*pg);
  UNITTEST_ASSERT_EQUAL(qzfull.steps(), 0);
  UNITTEST_ASSERT_FALSE(qzfull.check_for_null(pg));
  UNITTEST_ASSERT_EQUAL(qzfull.steps(), 1);
}

TEST(check_for_empty) {
  // A null graph.
  QuickZykov::GraphPtr pg(new Graph);
  QuickZykov qznull(*pg);
  UNITTEST_ASSERT_EQUAL(qznull.steps(), 0);
  UNITTEST_ASSERT_TRUE(qznull.check_for_empty(pg));
  UNITTEST_ASSERT_EQUAL(qznull.steps(), 1);

  // An empty graph.
  pg.reset(new Graph(5));
  QuickZykov qzempty(*pg);
  UNITTEST_ASSERT_EQUAL(qzempty.steps(), 0);
  UNITTEST_ASSERT_TRUE(qzempty.check_for_empty(pg));
  UNITTEST_ASSERT_EQUAL(qzempty.steps(), 1);

  // A full graph.
  pg.reset(new Graph(VERTICES, EDGES));
  QuickZykov qzfull(*pg);
  UNITTEST_ASSERT_EQUAL(qzfull.steps(), 0);
  UNITTEST_ASSERT_FALSE(qzfull.check_for_empty(pg));
  UNITTEST_ASSERT_EQUAL(qzfull.steps(), 1);
}

