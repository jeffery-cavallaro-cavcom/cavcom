// Uses the Bron-1 algorithm to find all maximal cliques in a graph.

#include <libunittest/all.hpp>

#include "bron1.h"

using namespace cavcom::graph;

static const VertexValuesList VERTICES = {{"a", NOCOLOR, 0, 2},
                                          {"b", NOCOLOR, 2, 2},
                                          {"c", NOCOLOR, 2, 0},
                                          {"d", NOCOLOR, 0, 0},
                                          {"e", NOCOLOR, 4, 2},
                                          {"f", NOCOLOR, 6, 1},
                                          {"g", NOCOLOR, 4, 0},
                                          {"h", NOCOLOR, 8, 1}};

static const EdgeValuesList EDGES = {{0, 1}, {0, 2}, {0, 3}, {0, 4},
                                     {1, 2}, {1, 3}, {1, 4}, {1, 6},
                                     {2, 3}, {2, 6},
                                     {3, 5},
                                     {4, 5}, {4, 6}, {4, 7},
                                     {5, 6}, {5, 7},
                                     {6, 7}};

static const Bron1::Cliques CLIQUES = {{0, 1, 2, 3},
                                       {0, 1, 4},
                                       {1, 2, 6},
                                       {1, 4, 6},
                                       {3, 5},
                                       {4, 5, 6, 7}};

TEST(null_graph) {
  SimpleGraph g;
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 0);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 0);
  UNITTEST_ASSERT_TRUE(b1.cliques().empty());
}

TEST(trivial_graph) {
  SimpleGraph g(1);
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 2);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0][0], 0);
}

TEST(empty_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 11);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), ORDER);
  for (Bron1::Cliques::size_type ic = 0; ic < ORDER; ++ic) {
    const Bron1::Clique &c = b1.cliques()[ic];
    UNITTEST_ASSERT_EQUAL(c.size(), 1);
    UNITTEST_ASSERT_EQUAL(c[0], ic);
  }
}

TEST(complete_graph) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), ORDER);
  for (Bron1::Clique::size_type ic = 0; ic < ORDER; ++ic) {
    UNITTEST_ASSERT_EQUAL(b1.cliques()[0][ic], ic);
  }
}

TEST(sample_graph) {
  SimpleGraph g(VERTICES, EDGES);
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), CLIQUES.size());
  for (Bron1::Cliques::size_type ic = 0; ic < CLIQUES.size(); ++ic) {
    UNITTEST_ASSERT_EQUAL_CONTAINERS(b1.cliques()[ic], CLIQUES[ic]);
  }
}
