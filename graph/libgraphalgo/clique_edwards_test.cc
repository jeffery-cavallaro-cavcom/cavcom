// Uses the Edwards algorithm to find a lower bound for the clique number.

#include <libunittest/all.hpp>

#include "clique_edwards.h"

#include "random_graph.h"
#include "bron2.h"

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
  CliqueEdwards ce(g, false);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 0);
  UNITTEST_ASSERT_EQUAL(ce.number(), 0);
}

TEST(null_graph_smart) {
  SimpleGraph g;
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 0);
  UNITTEST_ASSERT_EQUAL(ce.number(), 0);
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  CliqueEdwards ce(g, false);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 1);
  UNITTEST_ASSERT_EQUAL(ce.number(), 1);
}

TEST(trivial_graph_smart) {
  SimpleGraph g(1);
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 1);
  UNITTEST_ASSERT_EQUAL(ce.number(), 1);
}

TEST(empty_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  CliqueEdwards ce(g, false);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 20);
  UNITTEST_ASSERT_EQUAL(ce.number(), 1);
}

TEST(empty_graph_smart) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 20);
  UNITTEST_ASSERT_EQUAL(ce.number(), 1);
}

TEST(complete_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  CliqueEdwards ce(g, false);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 220);
  UNITTEST_ASSERT_EQUAL(ce.number(), ORDER);
}

TEST(complete_graph_smart) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 340);
  UNITTEST_ASSERT_EQUAL(ce.number(), ORDER);
}

TEST(sample_graph) {
  SimpleGraph g(VERTICES, EDGES);
  CliqueEdwards ce(g, false);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 44);
  UNITTEST_ASSERT_EQUAL(ce.number(), 4);
}

TEST(sample_graph_smart) {
  SimpleGraph g(VERTICES, EDGES);
  CliqueEdwards ce(g);
  UNITTEST_ASSERT_TRUE(ce.execute());
  UNITTEST_ASSERT_EQUAL(ce.steps(), 48);
  UNITTEST_ASSERT_EQUAL(ce.number(), 3);
}

TEST(random_graphs) {
  const VertexNumber ORDER = 10;
  for (uint ip = 10; ip <= 90; ip += 10) {
    RandomGraph g(ORDER, ip/100.0);
    CliqueEdwards ce(g);
    UNITTEST_ASSERT_TRUE(ce.execute());
    Bron2 b(g);
    UNITTEST_ASSERT_TRUE(b.execute());
    UNITTEST_ASSERT_GREATER(ce.number(), 0);
    UNITTEST_ASSERT_LESSER_EQUAL(ce.number(), b.number());
  }
}
