// Tests the parts and whole of the quick Zykov algorithm.

#include <libunittest/all.hpp>

#include "quick_zykov.h"

using namespace cavcom::graph;

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

TEST(null_graph) {
  QuickZykov::GraphPtr pg(new SimpleGraph);
  QuickZykov qz(*pg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.steps(), 1);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.k(), 0);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);
  UNITTEST_ASSERT_TRUE(qz.chromatic().null());
}

TEST(trivial_graph) {
  QuickZykov::GraphPtr pg(new SimpleGraph(1));
  QuickZykov qz(*pg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.steps(), 2);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.k(), 1);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(qz.chromatic().order(), 1);
  UNITTEST_ASSERT_EQUAL(qz.chromatic().size(), 0);
}

TEST(empty_graph) {
  QuickZykov::GraphPtr pg(new SimpleGraph(5));
  QuickZykov qz(*pg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.steps(), 2);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 0);
  UNITTEST_ASSERT_EQUAL(qz.k(), 1);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(qz.chromatic().order(), 5);
  UNITTEST_ASSERT_EQUAL(qz.chromatic().size(), 0);
}

TEST(complete_graph) {
  QuickZykov::GraphPtr pg(new SimpleGraph(5));
  pg->make_complete();
  QuickZykov qz(*pg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.steps(), 16);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 4);
  UNITTEST_ASSERT_EQUAL(qz.k(), pg->order());
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 1);
  UNITTEST_ASSERT_TRUE(qz.chromatic().complete());
}

TEST(sample_graph) {
  QuickZykov::GraphPtr pg(new SimpleGraph(VERTICES, EDGES));
  QuickZykov qz(*pg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.steps(), 52);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 5);
  UNITTEST_ASSERT_EQUAL(qz.k(), 3);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 2);

  const SimpleGraph &g = qz.chromatic();
  UNITTEST_ASSERT_EQUAL(g.order(), 3);
  UNITTEST_ASSERT_EQUAL(g.size(), 3);

  const Vertex &v0 = g.vertex(0);
  UNITTEST_ASSERT_EQUAL(v0.id(), 2);
  UNITTEST_ASSERT_TRUE(v0.contracted().empty());

  const Vertex &v1 = g.vertex(1);
  UNITTEST_ASSERT_EQUAL(v1.id(), 4);
  UNITTEST_ASSERT_TRUE(v1.contracted().empty());

  const Vertex &v2 = g.vertex(2);
  UNITTEST_ASSERT_EQUAL(v2.id(), 8);
  UNITTEST_ASSERT_EQUAL(v2.contracted().size(), 2);
  UNITTEST_ASSERT_EQUAL(v2.contracted()[0], 0);
  UNITTEST_ASSERT_EQUAL(v2.contracted()[1], 3);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 7);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 2);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 5);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 2);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 3);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 1);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 2);
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
  QuickZykov::GraphPtr pg(new SimpleGraph(VERTICES2, EDGES2));
  QuickZykov qz(*pg);
  UNITTEST_ASSERT_TRUE(qz.execute());
  UNITTEST_ASSERT_EQUAL(qz.steps(), 105);
  UNITTEST_ASSERT_EQUAL(qz.calls(), 9);
  UNITTEST_ASSERT_EQUAL(qz.k(), 4);
  UNITTEST_ASSERT_EQUAL(qz.maxdepth(), 4);

  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_tries(), 15);
  UNITTEST_ASSERT_EQUAL(qz.edge_threshold_hits(), 5);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_tries(), 10);
  UNITTEST_ASSERT_EQUAL(qz.small_degree_hits(), 2);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_tries(), 8);
  UNITTEST_ASSERT_EQUAL(qz.neighborhood_subset_hits(), 5);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_tries(), 3);
  UNITTEST_ASSERT_EQUAL(qz.common_neighbors_hits(), 0);
}
