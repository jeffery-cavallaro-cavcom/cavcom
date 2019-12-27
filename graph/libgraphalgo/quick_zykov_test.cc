// Tests the parts and whole of the quick Zykov algorithm.

#include <libunittest/all.hpp>

#include "quick_zykov.h"

#include "mycielski.h"
#include "random_graph.h"

using namespace cavcom::graph;

TEST(null_graph) {
  SimpleGraph g;
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.kmin(), 0);
  UNITTEST_ASSERT_EQUAL(qz.kmax(), 0);
  UNITTEST_ASSERT_EQUAL(qz.number(), 0);
  UNITTEST_ASSERT_TRUE(qz.coloring().empty());

  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_hits(), 0);
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.kmin(), 1);
  UNITTEST_ASSERT_EQUAL(qz.kmax(), 1);
  UNITTEST_ASSERT_EQUAL(qz.number(), 1);
  UNITTEST_ASSERT_EQUAL(qz.coloring().size(), 1);
  VertexNumbersList expected = {{0}};
  UNITTEST_ASSERT_EQUAL_CONTAINERS(qz.coloring(), expected);

  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_hits(), 0);
}

TEST(empty_graph) {
  constexpr VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.kmin(), 1);
  UNITTEST_ASSERT_EQUAL(qz.kmax(), 1);
  UNITTEST_ASSERT_EQUAL(qz.number(), 1);
  VertexNumbers numbers;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) numbers.insert(iv);
  VertexNumbersList expected = { numbers };
  UNITTEST_ASSERT_EQUAL(qz.coloring().size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(qz.coloring(), expected);

  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_hits(), 0);
}

TEST(complete_graph) {
  constexpr VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.kmin(), ORDER);
  UNITTEST_ASSERT_EQUAL(qz.kmax(), ORDER);
  UNITTEST_ASSERT_EQUAL(qz.number(), ORDER);
  VertexNumbersList expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.push_back({ iv });
  VertexNumbersList actual = qz.coloring();
  std::sort(actual.begin(), actual.end());
  UNITTEST_ASSERT_EQUAL(actual.size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(actual, expected);

  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_hits(), 0);
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
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.kmin(), 3);
  UNITTEST_ASSERT_EQUAL(qz.kmax(), 4);
  UNITTEST_ASSERT_EQUAL(qz.number(), COLORING.size());
  VertexNumbersList actual = qz.coloring();
  std::sort(actual.begin(), actual.end());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(actual, COLORING);

  UNITTEST_ASSERT_EQUAL(qz.steps(), 19);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 1);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 1);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 1);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 1);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 1);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_hits(), 0);

  UNITTEST_ASSERT_FALSE(g.proper());
  qz.apply(&g);
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
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.kmin(), 4);
  UNITTEST_ASSERT_EQUAL(qz.kmax(), 4);
  UNITTEST_ASSERT_EQUAL(qz.number(), COLORING2.size());
  VertexNumbersList actual = qz.coloring();
  std::sort(actual.begin(), actual.end());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(actual, COLORING2);

  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_hits(), 0);

  UNITTEST_ASSERT_FALSE(g.proper());
  qz.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
}

static const VertexNumbersList COLORINGM = {{0, 2, 10}, {1, 3, 6, 8}, {4, 5, 9}, {7}};

TEST(myclieski_graph) {
  Mycielski g(4);
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.kmin(), 2);
  UNITTEST_ASSERT_EQUAL(qz.kmax(), 4);
  UNITTEST_ASSERT_EQUAL(qz.number(), COLORINGM.size());
  VertexNumbersList actual = qz.coloring();
  std::sort(actual.begin(), actual.end());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(actual, COLORINGM);

  UNITTEST_ASSERT_EQUAL(qz.steps(), 306);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 20);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 2);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 47);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 15);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 32);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 14);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 18);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 13);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 5);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.bounding_tries(), 5);
  UNITTEST_ASSERT_EQUAL(qz.bounding_hits(), 0);

  UNITTEST_ASSERT_FALSE(g.proper());
  qz.apply(&g);
  UNITTEST_ASSERT_TRUE(g.proper());
}

TEST(random_graphs) {
  const VertexNumber ORDER = 10;
  const uint TRIALS = 100;
  for (uint ip = 10; ip <= 90; ip += 10) {
    for (uint it = 0; it < TRIALS; ++it) {
      RandomGraph g(ORDER, ip/100.0);
      QuickZykov qz(g);
      qz.execute();
      qz.apply(&g);
      UNITTEST_ASSERT_TRUE(g.proper());
    }
  }
}
