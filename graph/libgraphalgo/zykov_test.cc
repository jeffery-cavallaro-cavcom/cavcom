// Runs the Zykov chromatic number algorithm.

#include <libunittest/all.hpp>

#include "zykov.h"

#include "mycielski.h"
#include "random_graph.h"

using namespace cavcom::graph;

TEST(null_graph) {
  SimpleGraph g(0);
  Zykov z(g);
  UNITTEST_ASSERT_TRUE(z.execute());
  UNITTEST_ASSERT_EQUAL(z.calls(), 1);
  UNITTEST_ASSERT_EQUAL(z.number(), 0);
  UNITTEST_ASSERT_EQUAL(z.coloring().size(), 0);
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  Zykov z(g);
  UNITTEST_ASSERT_TRUE(z.execute());
  UNITTEST_ASSERT_EQUAL(z.calls(), 1);
  UNITTEST_ASSERT_EQUAL(z.number(), 1);
  VertexNumbersList expected = {{0}};
  UNITTEST_ASSERT_EQUAL(z.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(z.coloring(), expected);
}

TEST(empty_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Zykov z(g);
  UNITTEST_ASSERT_TRUE(z.execute());
  UNITTEST_ASSERT_EQUAL(z.calls(), 1);
  UNITTEST_ASSERT_EQUAL(z.number(), 1);
  VertexNumbers numbers;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) numbers.insert(iv);
  VertexNumbersList expected = { numbers };
  UNITTEST_ASSERT_EQUAL(z.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(z.coloring(), expected);
}

TEST(complete_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Zykov z(g);
  UNITTEST_ASSERT_TRUE(z.execute());
  UNITTEST_ASSERT_EQUAL(z.calls(), 1);
  UNITTEST_ASSERT_EQUAL(z.number(), ORDER);
  VertexNumbersList expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.push_back({ iv });
  VertexNumbersList actual = z.coloring();
  std::sort(actual.begin(), actual.end());
  UNITTEST_ASSERT_EQUAL(actual.size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(actual, expected);
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

static const VertexNumbersList COLORING = {{0, 3}, {1, 4, 6, 7}, {2, 5}};

TEST(sample_graph) {
  SimpleGraph g(VERTICES, EDGES);
  Zykov z(g);
  UNITTEST_ASSERT_TRUE(z.execute());
  UNITTEST_ASSERT_EQUAL(z.calls(), 31);
  UNITTEST_ASSERT_EQUAL(z.number(), COLORING.size());
  VertexNumbersList actual = z.coloring();
  std::sort(actual.begin(), actual.end());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(actual, COLORING);

  UNITTEST_ASSERT_FALSE(g.proper());
  z.apply(&g);
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
  Zykov z(g);
  UNITTEST_ASSERT_TRUE(z.execute());
  UNITTEST_ASSERT_EQUAL(z.calls(), 7);
  UNITTEST_ASSERT_EQUAL(z.number(), COLORING2.size());
  UNITTEST_ASSERT_EQUAL(z.coloring().size(), COLORING2.size());
  VertexNumbersList actual = z.coloring();
  std::sort(actual.begin(), actual.end());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(actual, COLORING2);

  UNITTEST_ASSERT_FALSE(g.proper());
  z.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
}

static const VertexNumbersList COLORINGM = {{0, 2, 10}, {1, 3, 6, 8}, {4, 5, 9}, {7}};

TEST(myclieski_graph) {
  Mycielski g(4);
  Zykov z(g);
  UNITTEST_ASSERT_TRUE(z.execute());
  UNITTEST_ASSERT_EQUAL(z.calls(), 175);
  UNITTEST_ASSERT_EQUAL(z.number(), COLORINGM.size());
  VertexNumbersList actual = z.coloring();
  std::sort(actual.begin(), actual.end());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(actual, COLORINGM);

  UNITTEST_ASSERT_FALSE(g.proper());
  z.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
}

TEST(random_graphs) {
  const VertexNumber ORDER = 10;
  const uint TRIALS = 100;
  for (uint ip = 10; ip <= 90; ip += 10) {
    for (uint it = 0; it < TRIALS; ++it) {
      RandomGraph g(ORDER, ip/100.0);
      Zykov z(g);
      UNITTEST_ASSERT_TRUE(z.execute());
      UNITTEST_ASSERT_FALSE(g.proper());
      z.apply(&g);
      UNITTEST_ASSERT_TRUE(g.proper());
    }
  }
}
