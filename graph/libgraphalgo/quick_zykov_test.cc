// Tests the parts and whole of the quick Zykov algorithm.

#include <libunittest/all.hpp>

#include "quick_zykov.h"

using namespace cavcom::graph;

TEST(null_graph) {
  SimpleGraph g;
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.lower_bound(), 0);
  UNITTEST_ASSERT_EQUAL(qz.upper_bound(), 0);
  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.number(), 0);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.lower_bound(), 1);
  UNITTEST_ASSERT_EQUAL(qz.upper_bound(), 1);
  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.number(), 1);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
}

TEST(empty_graph) {
  constexpr VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.lower_bound(), 1);
  UNITTEST_ASSERT_EQUAL(qz.upper_bound(), 1);
  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.number(), 1);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
}

TEST(complete_graph) {
  constexpr VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.lower_bound(), ORDER);
  UNITTEST_ASSERT_EQUAL(qz.upper_bound(), ORDER);
  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
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

TEST(sample_graph) {
  SimpleGraph g(VERTICES, EDGES);
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.lower_bound(), 3);
  UNITTEST_ASSERT_EQUAL(qz.upper_bound(), 4);
  UNITTEST_ASSERT_EQUAL(qz.steps(), 25);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 2);
  UNITTEST_ASSERT_EQUAL(qz.number(), 3);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 2);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 3);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 3);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 2);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 1);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 1);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
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

TEST(sample_graph_2) {
  SimpleGraph g(VERTICES2, EDGES2);
  QuickZykov qz(g);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.lower_bound(), 4);
  UNITTEST_ASSERT_EQUAL(qz.upper_bound(), 4);
  UNITTEST_ASSERT_EQUAL(qz.steps(), 3);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.number(), 4);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 0);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
}
