// Runs the Wang chromatic number algorithm.

#include <algorithm>

#include <libunittest/all.hpp>

#include "chromatic_wang.h"

#include "random_graph.h"

using namespace cavcom::graph;

TEST(null_graph) {
  SimpleGraph g(0);
  ChromaticWang cw(g);
  UNITTEST_ASSERT_TRUE(cw.execute());
  UNITTEST_ASSERT_EQUAL(cw.calls(), 0);
  UNITTEST_ASSERT_EQUAL(cw.number(), 0);
  UNITTEST_ASSERT_EQUAL(cw.coloring().size(), 0);
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  ChromaticWang cw(g);
  UNITTEST_ASSERT_TRUE(cw.execute());
  UNITTEST_ASSERT_EQUAL(cw.calls(), 1);
  UNITTEST_ASSERT_EQUAL(cw.number(), 1);
  VertexNumbersList expected = {{0}};
  UNITTEST_ASSERT_EQUAL(cw.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cw.coloring(), expected);
}

TEST(empty_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  ChromaticWang cw(g);
  UNITTEST_ASSERT_TRUE(cw.execute());
  UNITTEST_ASSERT_EQUAL(cw.calls(), 1);
  UNITTEST_ASSERT_EQUAL(cw.number(), 1);
  VertexNumbers expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.insert(iv);
  UNITTEST_ASSERT_EQUAL(cw.coloring().size(), 1);
  UNITTEST_ASSERT_EQUAL(cw.coloring()[0].size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cw.coloring()[0], expected);
}

TEST(complete_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  ChromaticWang cw(g);
  UNITTEST_ASSERT_TRUE(cw.execute());
  UNITTEST_ASSERT_EQUAL(cw.calls(), 10);
  UNITTEST_ASSERT_EQUAL(cw.number(), ORDER);
  VertexNumbersList expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    VertexNumbers part = {iv};
    expected.push_back(part);
  }
  VertexNumbersList found = cw.coloring();
  std::sort(found.begin(), found.end());
  UNITTEST_ASSERT_EQUAL(found.size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(found, expected);
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

static const VertexNumbersList COLORING = {{0, 4, 6, 7}, {1, 5}, {2, 3}};

TEST(sample_graph) {
  SimpleGraph g(VERTICES, EDGES);
  ChromaticWang cw(g);
  UNITTEST_ASSERT_TRUE(cw.execute());
  UNITTEST_ASSERT_EQUAL(cw.calls(), 6);
  UNITTEST_ASSERT_EQUAL(cw.number(), COLORING.size());
  VertexNumbersList found = cw.coloring();
  std::sort(found.begin(), found.end());
  UNITTEST_ASSERT_EQUAL(found.size(), COLORING.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(found, COLORING);
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

static const VertexNumbersList COLORING2 = {{0, 6, 8}, {1, 7}, {2, 5}, {3, 4}};

TEST(sample_graph_2) {
  SimpleGraph g(VERTICES2, EDGES2);
  ChromaticWang cw(g);
  UNITTEST_ASSERT_TRUE(cw.execute());
  UNITTEST_ASSERT_EQUAL(cw.calls(), 15);
  UNITTEST_ASSERT_EQUAL(cw.number(), COLORING2.size());
  VertexNumbersList found = cw.coloring();
  std::sort(found.begin(), found.end());
  UNITTEST_ASSERT_EQUAL(found.size(), COLORING2.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(found, COLORING2);
}

TEST(random_graphs) {
  const VertexNumber ORDER = 10;
  const uint TRIALS = 100;
  for (uint ip = 10; ip <= 90; ip += 10) {
    for (uint it = 0; it < TRIALS; ++it) {
      RandomGraph g(ORDER, ip/100.0);
      ChromaticWang cw(g);
      UNITTEST_ASSERT_TRUE(cw.execute());
      UNITTEST_ASSERT_FALSE(g.proper());
      cw.apply(&g);
      UNITTEST_ASSERT_TRUE(g.proper());
    }
  }
}
