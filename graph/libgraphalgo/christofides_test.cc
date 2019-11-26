// Tests the parts and whole of the Christofides chromatic number algorithm.

#include <algorithm>

#include <libunittest/all.hpp>

#include "christofides.h"

using namespace cavcom::graph;

TEST(null_graph) {
  SimpleGraph g(0);
  Christofides c(g);
  UNITTEST_ASSERT_TRUE(c.execute());
  UNITTEST_ASSERT_EQUAL(c.calls(), 0);
  UNITTEST_ASSERT_EQUAL(c.number(), 0);
  UNITTEST_ASSERT_EQUAL(c.chromatic().size(), 0);
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  Christofides c(g);
  UNITTEST_ASSERT_TRUE(c.execute());
  UNITTEST_ASSERT_EQUAL(c.calls(), 1);
  UNITTEST_ASSERT_EQUAL(c.number(), 1);
  UNITTEST_ASSERT_EQUAL(c.chromatic().size(), 1);
  UNITTEST_ASSERT_FALSE(c.chromatic()[0].find(0) == c.chromatic()[0].cend());
}

TEST(empty_graph) {
  const VertexNumber ORDER = 5;
  SimpleGraph g(ORDER);
  Christofides c(g);
  UNITTEST_ASSERT_TRUE(c.execute());
  UNITTEST_ASSERT_EQUAL(c.calls(), 1);
  UNITTEST_ASSERT_EQUAL(c.number(), 1);
  UNITTEST_ASSERT_EQUAL(c.chromatic().size(), 1);
  UNITTEST_ASSERT_EQUAL(c.chromatic()[0].size(), ORDER);
  for (VertexID iv = 0; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_FALSE(c.chromatic()[0].find(iv) == c.chromatic()[0].cend());
  }
}

TEST(complete_graph) {
  const VertexNumber ORDER = 5;
  SimpleGraph g(ORDER);
  g.make_complete();
  Christofides c(g);
  UNITTEST_ASSERT_TRUE(c.execute());
  UNITTEST_ASSERT_EQUAL(c.calls(), 76);
  UNITTEST_ASSERT_EQUAL(c.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(c.chromatic().size(), ORDER);
  std::vector<bool> found(ORDER);
  for_each(c.chromatic().cbegin(), c.chromatic().cend(), [&](Christofides::IDs mis){
                                                           UNITTEST_ASSERT_EQUAL(mis.size(), 1);
                                                           VertexID id = *mis.cbegin();
                                                           UNITTEST_ASSERT_FALSE(found[id]);
                                                           found[id] = true;
                                                         });
  for (VertexID v = 0; v < ORDER; ++v) {
    UNITTEST_ASSERT_TRUE(found[v]);
  }
}

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

static const Christofides::Coloring COLORING = {{0, 4, 6, 7}, {1, 5}, {2, 3}};

TEST(sample_graph) {
  SimpleGraph g(VERTICES, EDGES);
  Christofides c(g);
  UNITTEST_ASSERT_TRUE(c.execute());
  UNITTEST_ASSERT_EQUAL(c.calls(), 32);
  UNITTEST_ASSERT_EQUAL(c.number(), 3);
  UNITTEST_ASSERT_EQUAL(c.chromatic().size(), 3);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(c.chromatic(), COLORING);
}

static const VertexValuesList VERTICES2 = {{"a", NOCOLOR, 0, 2},
                                           {"b", NOCOLOR, 2, 2},
                                           {"c", NOCOLOR, 2, 0},
                                           {"d", NOCOLOR, 0, 0},
                                           {"e", NOCOLOR, 4, 2},
                                           {"f", NOCOLOR, 6, 1},
                                           {"g", NOCOLOR, 4, 0},
                                           {"h", NOCOLOR, 8, 1},
                                           {"i", NOCOLOR, 10, 1}};

static const EdgeValuesList EDGES2 = {{0, 1}, {0, 2}, {0, 3}, {0, 4},
                                      {1, 2}, {1, 3}, {1, 4}, {1, 6},
                                      {2, 3}, {2, 6},
                                      {3, 6},
                                      {4, 5}, {4, 6}, {4, 7},
                                      {5, 6}, {5, 7},
                                      {6, 7},
                                      {7, 8}};

static const Christofides::Coloring COLORING2 = {{2, 5, 8}, {1, 7}, {0, 6}, {3, 4}};

TEST(sample_graph_2) {
  SimpleGraph g(VERTICES2, EDGES2);
  Christofides c(g);
  UNITTEST_ASSERT_TRUE(c.execute());
  UNITTEST_ASSERT_EQUAL(c.calls(), 158);
  UNITTEST_ASSERT_EQUAL(c.number(), 4);
  UNITTEST_ASSERT_EQUAL(c.chromatic().size(), 4);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(c.chromatic(), COLORING2);
}
