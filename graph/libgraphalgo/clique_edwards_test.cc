// Uses the Edwards algorithm to find a lower bound for the clique number.

#include <libunittest/all.hpp>

#include "clique_edwards.h"

using namespace cavcom::graph;

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

TEST(null_graph) {
  SimpleGraph g;
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 0);
  UNITTEST_ASSERT_TRUE(ce.clique().empty());
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 2);
  UNITTEST_ASSERT_EQUAL(ce.clique().size(), 1);
  VertexNumbers expected = {0};
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ce.clique(), expected);
}

TEST(empty_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 20);
  UNITTEST_ASSERT_EQUAL(ce.clique().size(), 1);
  VertexNumbers expected = {0};
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ce.clique(), expected);
}

TEST(complete_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 110);
  UNITTEST_ASSERT_EQUAL(ce.clique().size(), ORDER);
  VertexNumbers expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.insert(iv);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ce.clique(), expected);
}

TEST(sample_all) {
  SimpleGraph g(VERTICES, EDGES);
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 40);
  VertexNumbers expected = {1, 2, 3, 6};
  UNITTEST_ASSERT_EQUAL(ce.clique().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ce.clique(), expected);
}
