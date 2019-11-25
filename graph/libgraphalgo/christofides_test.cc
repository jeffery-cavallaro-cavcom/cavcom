// Tests the parts and whole of the Christofides chromatic number algorithm.

#include <libunittest/all.hpp>

#include "christofides.h"

using namespace cavcom::graph;

static const VertexValuesList VERTICES = {{"a", NOCOLOR, 2, 6},
                                          {"b", NOCOLOR, 0, 4},
                                          {"c", NOCOLOR, 4, 4},
                                          {"d", NOCOLOR, 0, 2},
                                          {"e", NOCOLOR, 4, 2},
                                          {"f", NOCOLOR, 2, 0},
                                          {"g", NOCOLOR, 0, 0},
                                          {"h", NOCOLOR, 4, 0}};

static const EdgeValuesList EDGES = {{0, 1}, {0, 2}, {0, 5},
                                     {1, 2}, {1, 3},
                                     {2, 4},
                                     {3, 4}, {3, 5}, {3, 6},
                                     {4, 5},
                                     {5, 6}, {5, 7}};

TEST(sample_graph) {
  SimpleGraph g(VERTICES, EDGES);
  Christofides c(g);
  UNITTEST_ASSERT_TRUE(c.execute());
  UNITTEST_ASSERT_EQUAL(c.calls(), 0);
  UNITTEST_ASSERT_EQUAL(c.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(c.number(), 3);
  UNITTEST_ASSERT_EQUAL(c.chromatic().size(), 3);
}
