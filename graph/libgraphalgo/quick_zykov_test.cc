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
                                     

TEST(null_graph) {
  QuickZykov::GraphPtr pg(new SimpleGraph);
  QuickZykov qz(*pg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.steps(), 1);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.k(), 0);
  UNITTEST_ASSERT_TRUE(qz.chromatic().null());
}

TEST(empty_graph) {
  QuickZykov::GraphPtr pg(new SimpleGraph(5));
  QuickZykov qz(*pg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.steps(), 2);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.k(), 1);
  UNITTEST_ASSERT_TRUE(qz.chromatic().empty());
}

TEST(complete_graph) {
  QuickZykov::GraphPtr pg(new SimpleGraph(5));
  pg->make_complete();
  QuickZykov qz(*pg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.steps(), 16);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 4);
  UNITTEST_ASSERT_EQUAL(qz.k(), pg->order());
  UNITTEST_ASSERT_TRUE(qz.chromatic().complete());
}
