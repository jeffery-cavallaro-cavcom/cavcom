// Runs the Hopcroft Tarjan component algorithm.

#include <libunittest/all.hpp>

#include "hopcroft.h"

using namespace cavcom::graph;

TEST(null_graph) {
  SimpleGraph g(0);
  Hopcroft hop(g);
  UNITTEST_ASSERT_TRUE(hop.execute());
  UNITTEST_ASSERT_EQUAL(hop.calls(), g.order());
  UNITTEST_ASSERT_EQUAL(hop.maxdepth(), 0);
  UNITTEST_ASSERT_TRUE(hop.components().empty());
  UNITTEST_ASSERT_EQUAL(hop.number(), 0);
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  Hopcroft hop(g);
  UNITTEST_ASSERT_TRUE(hop.execute());
  UNITTEST_ASSERT_EQUAL(hop.calls(), g.order());
  UNITTEST_ASSERT_EQUAL(hop.maxdepth(), 1);
  UNITTEST_ASSERT_EQUAL(hop.components().size(), 1);
  UNITTEST_ASSERT_EQUAL(hop.number(), 1);
  VertexNumbersList expected = {{0}};
  UNITTEST_ASSERT_EQUAL_CONTAINERS(hop.components(), expected);
}

TEST(empty_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Hopcroft hop(g);
  UNITTEST_ASSERT_TRUE(hop.execute());
  UNITTEST_ASSERT_EQUAL(hop.calls(), g.order());
  UNITTEST_ASSERT_EQUAL(hop.maxdepth(), 1);
  UNITTEST_ASSERT_EQUAL(hop.components().size(), ORDER);
  UNITTEST_ASSERT_EQUAL(hop.number(), ORDER);
  VertexNumbersList expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.push_back({iv});
  UNITTEST_ASSERT_EQUAL_CONTAINERS(hop.components(), expected);
}

TEST(complete_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Hopcroft hop(g);
  UNITTEST_ASSERT_TRUE(hop.execute());
  UNITTEST_ASSERT_EQUAL(hop.calls(), g.order());
  UNITTEST_ASSERT_EQUAL(hop.maxdepth(), ORDER);
  UNITTEST_ASSERT_EQUAL(hop.components().size(), 1);
  UNITTEST_ASSERT_EQUAL(hop.number(), 1);
  VertexNumbersList expected;
  expected.push_back(VertexNumbers());
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.back().insert(iv);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(hop.components(), expected);
}

static const VertexValuesList VERTICES = {{"a", NOCOLOR, 0, 2},
                                          {"b", NOCOLOR, 2, 2},
                                          {"c", NOCOLOR, 2, 0},
                                          {"d", NOCOLOR, 0, 0},
                                          {"e", NOCOLOR, 4, 2},
                                          {"f", NOCOLOR, 6, 1},
                                          {"g", NOCOLOR, 4, 0},
                                          {"h", NOCOLOR, 8, 1},
                                          {"i", NOCOLOR, 10, 1}};

static const EdgeValuesList EDGES = {{0, 1}, {0, 2}, {0, 3}, {0, 4},
                                     {1, 2}, {1, 3}, {1, 4}, {1, 6},
                                     {2, 3}, {2, 6},
                                     {3, 6},
                                     {4, 5}, {4, 6}, {4, 7},
                                     {5, 6}, {5, 7},
                                     {6, 7},
                                     {7, 8}};

TEST(connected_graph) {
  SimpleGraph g(VERTICES, EDGES);
  Hopcroft hop(g);
  UNITTEST_ASSERT_TRUE(hop.execute());
  UNITTEST_ASSERT_EQUAL(hop.calls(), g.order());
  UNITTEST_ASSERT_EQUAL(hop.maxdepth(), g.order());
  UNITTEST_ASSERT_EQUAL(hop.components().size(), 1);
  UNITTEST_ASSERT_EQUAL(hop.number(), 1);
  VertexNumbersList expected;
  expected.push_back(VertexNumbers());
  for (VertexNumber iv = 0; iv < g.order(); ++iv) expected.back().insert(iv);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(hop.components(), expected);
}

static const EdgeValuesList EDGES2 = {{0, 3}, {0, 8}, {3, 8}, {8, 6},
                                      {1, 2}, {2, 7}, {7, 5}, {5, 1}};

static const VertexNumbersList COMPONENTS = {{0, 3, 6, 8}, {1, 2, 5, 7}, {4}};

TEST(disconnected_graph) {
  SimpleGraph g(VERTICES, EDGES2);
  Hopcroft hop(g);
  UNITTEST_ASSERT_TRUE(hop.execute());
  UNITTEST_ASSERT_EQUAL(hop.calls(), g.order());
  UNITTEST_ASSERT_EQUAL(hop.maxdepth(), 4);
  UNITTEST_ASSERT_EQUAL(hop.components().size(), COMPONENTS.size());
  UNITTEST_ASSERT_EQUAL(hop.number(), COMPONENTS.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(hop.components(), COMPONENTS);
}
