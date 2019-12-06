// Runs the last-first greedy coloring algorithm.

#include <libunittest/all.hpp>

#include "greedy_coloring_lf.h"

#include "random_graph.h"

using namespace cavcom::graph;

TEST(null_graph) {
  SimpleGraph g(0);
  GreedyColoringLF glf(g);
  UNITTEST_ASSERT_TRUE(glf.execute());
  UNITTEST_ASSERT_EQUAL(glf.steps(), 0);
  UNITTEST_ASSERT_EQUAL(glf.number(), 0);
  UNITTEST_ASSERT_EQUAL(glf.coloring().size(), 0);
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  GreedyColoringLF glf(g);
  UNITTEST_ASSERT_TRUE(glf.execute());
  UNITTEST_ASSERT_EQUAL(glf.steps(), 0);
  UNITTEST_ASSERT_EQUAL(glf.number(), 1);
  GreedyColoringLF::Coloring expected = {{0}};
  UNITTEST_ASSERT_EQUAL(glf.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(glf.coloring(), expected);
}

TEST(empty_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  GreedyColoringLF glf(g);
  UNITTEST_ASSERT_TRUE(glf.execute());
  UNITTEST_ASSERT_EQUAL(glf.steps(), 45);
  UNITTEST_ASSERT_EQUAL(glf.number(), 1);
  VertexNumbers expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.insert(iv);
  UNITTEST_ASSERT_EQUAL(glf.coloring().size(), 1);
  UNITTEST_ASSERT_EQUAL(glf.coloring()[0].size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(glf.coloring()[0], expected);
}

TEST(complete_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  GreedyColoringLF glf(g);
  UNITTEST_ASSERT_TRUE(glf.execute());
  UNITTEST_ASSERT_EQUAL(glf.steps(), 90);
  UNITTEST_ASSERT_EQUAL(glf.number(), ORDER);
  GreedyColoringLF::Coloring expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    VertexNumbers part = {iv};
    expected.push_back(part);
  }
  UNITTEST_ASSERT_EQUAL(glf.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(glf.coloring(), expected);
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

static const GreedyColoringLF::Coloring COLORING = {{1, 5}, {0, 3, 7}, {2, 6}, {4}};

TEST(sample_graph) {
  SimpleGraph g(VERTICES, EDGES);
  GreedyColoringLF glf(g);
  UNITTEST_ASSERT_TRUE(glf.execute());
  UNITTEST_ASSERT_EQUAL(glf.steps(), 38);
  UNITTEST_ASSERT_EQUAL(glf.number(), COLORING.size());
  UNITTEST_ASSERT_EQUAL(glf.coloring().size(), COLORING.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(glf.coloring(), COLORING);
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

static const GreedyColoringLF::Coloring COLORING2 = {{0, 6, 8}, {1, 7}, {2, 4}, {3, 5}};

TEST(sample_graph_2) {
  SimpleGraph g(VERTICES2, EDGES2);
  GreedyColoringLF glf(g);
  UNITTEST_ASSERT_TRUE(glf.execute());
  UNITTEST_ASSERT_EQUAL(glf.steps(), 48);
  UNITTEST_ASSERT_EQUAL(glf.number(), COLORING2.size());
  UNITTEST_ASSERT_EQUAL(glf.coloring().size(), COLORING2.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(glf.coloring(), COLORING2);
}

TEST(random_graphs) {
  const VertexNumber ORDER = 10;
  for (uint ip = 10; ip <= 90; ip += 10) {
    RandomGraph g(ORDER, ip/100.0);
    GreedyColoringLF glf(g);
    UNITTEST_ASSERT_TRUE(glf.execute());
    UNITTEST_ASSERT_FALSE(g.proper());
    glf.apply(&g);
    UNITTEST_ASSERT_TRUE(g.proper());
  }
}
