// Runs the default last-first greedy coloring algorithm.

#include <libunittest/all.hpp>

#include "greedy_coloring.h"

#include "random_graph.h"

using namespace cavcom::graph;

TEST(null_graph) {
  SimpleGraph g(0);
  GreedyColoring gc(g, false);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 0);
  UNITTEST_ASSERT_EQUAL(gc.number(), 0);
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), 0);
}

TEST(null_interchange) {
  SimpleGraph g(0);
  GreedyColoring gc(g);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 0);
  UNITTEST_ASSERT_EQUAL(gc.number(), 0);
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), 0);
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  GreedyColoring gc(g, false);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 0);
  UNITTEST_ASSERT_EQUAL(gc.number(), 1);
  VertexNumbersList expected = {{0}};
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), expected);
}

TEST(trivial_interchange) {
  SimpleGraph g(1);
  GreedyColoring gc(g);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 0);
  UNITTEST_ASSERT_EQUAL(gc.number(), 1);
  VertexNumbersList expected = {{0}};
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), expected);
}

TEST(empty_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  GreedyColoring gc(g, false);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 45);
  UNITTEST_ASSERT_EQUAL(gc.number(), 1);
  VertexNumbers expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.insert(iv);
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), 1);
  UNITTEST_ASSERT_EQUAL(gc.coloring()[0].size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring()[0], expected);
}

TEST(empty_interchange) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  GreedyColoring gc(g);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 45);
  UNITTEST_ASSERT_EQUAL(gc.number(), 1);
  VertexNumbers expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.insert(iv);
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), 1);
  UNITTEST_ASSERT_EQUAL(gc.coloring()[0].size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring()[0], expected);
}

TEST(complete_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  GreedyColoring gc(g, false);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 90);
  UNITTEST_ASSERT_EQUAL(gc.number(), ORDER);
  VertexNumbersList expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    VertexNumbers part = {iv};
    expected.push_back(part);
  }
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), expected);
}

TEST(complete_interchange) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  GreedyColoring gc(g);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 252);
  UNITTEST_ASSERT_EQUAL(gc.number(), ORDER);
  VertexNumbersList expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    VertexNumbers part = {iv};
    expected.push_back(part);
  }
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), expected);
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

static const VertexNumbersList COLORING = {{1, 5}, {0, 3, 7}, {2, 6}, {4}};

TEST(sample_graph) {
  SimpleGraph g(VERTICES, EDGES);
  GreedyColoring gc(g, false);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 38);
  UNITTEST_ASSERT_EQUAL(gc.number(), COLORING.size());
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), COLORING.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), COLORING);
  gc.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
}

TEST(sample_interchange) {
  SimpleGraph g(VERTICES, EDGES);
  GreedyColoring gc(g);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 68);
  UNITTEST_ASSERT_EQUAL(gc.number(), COLORING.size());
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), COLORING.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), COLORING);
  gc.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
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

static const VertexNumbersList COLORING2 = {{0, 6, 8}, {1, 7}, {2, 4}, {3, 5}};

TEST(sample_graph_2) {
  SimpleGraph g(VERTICES2, EDGES2);
  GreedyColoring gc(g, false);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 48);
  UNITTEST_ASSERT_EQUAL(gc.number(), COLORING2.size());
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), COLORING2.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), COLORING2);
  gc.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
}

TEST(sample_interchange_2) {
  SimpleGraph g(VERTICES2, EDGES2);
  GreedyColoring gc(g);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 64);
  UNITTEST_ASSERT_EQUAL(gc.number(), COLORING2.size());
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), COLORING2.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), COLORING2);
  gc.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
}

static const VertexValuesList VERTICES3 = {{"a"}, {"b"}, {"c"}, {"d"}, {"e"},
                                           {"f"}, {"g"}, {"h"}, {"i"}, {"j"}};

static const EdgeValuesList EDGES3 = {{0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9},
                                      {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8},
                                      {2, 4}, {2, 5}, {2, 6}, {2, 7}, {2, 8},
                                      {3, 4}, {3, 6}, {3, 7}, {3, 8},
                                      {4, 5}, {4, 8}, {4, 9},
                                      {5, 6}, {5, 8},
                                      {6, 7},
                                      {7, 9},
                                      {8, 9}};

static const VertexNumbersList COLORING3 = {{6, 8}, {4, 7}, {3, 5, 9}, {0, 1, 2}};
static const VertexNumbersList ICOLORING3 = {{6, 8}, {5, 7}, {0, 4}, {1, 2, 3, 9}};

TEST(sample_graph_3) {
  SimpleGraph g(VERTICES3, EDGES3);
  GreedyColoring gc(g, false);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 62);
  UNITTEST_ASSERT_EQUAL(gc.number(), COLORING3.size());
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), COLORING3.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), COLORING3);
  gc.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
}

TEST(sample_interchange_3) {
  SimpleGraph g(VERTICES3, EDGES3);
  GreedyColoring gc(g);
  UNITTEST_ASSERT_TRUE(gc.execute());
  UNITTEST_ASSERT_EQUAL(gc.steps(), 100);
  UNITTEST_ASSERT_EQUAL(gc.number(), ICOLORING3.size());
  UNITTEST_ASSERT_EQUAL(gc.coloring().size(), ICOLORING3.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(gc.coloring(), ICOLORING3);
  gc.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
}

TEST(random_graphs) {
  const VertexNumber ORDER = 50;
  const uint TRIALS = 100;
  for (uint ip = 10; ip <= 90; ip += 10) {
    for (uint it = 0; it < TRIALS; ++it) {
      RandomGraph g(ORDER, ip/100.0);
      GreedyColoring gc(g, false);
      UNITTEST_ASSERT_TRUE(gc.execute());
      GreedyColoring gci(g);
      UNITTEST_ASSERT_TRUE(gci.execute());
      UNITTEST_ASSERT_FALSE(g.proper());
      gc.apply(&g);
      UNITTEST_ASSERT_TRUE(g.proper());
      gci.apply(&g);
      UNITTEST_ASSERT_TRUE(g.proper());
    }
  }
}
