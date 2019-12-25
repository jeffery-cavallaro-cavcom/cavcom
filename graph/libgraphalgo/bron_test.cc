// Uses the Bron Kerbosh algorithm to find all maximal cliques in a graph.

#include <algorithm>

#include <libunittest/all.hpp>

#include "bron1.h"
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

static const VertexNumbersList CLIQUES = {{0, 1, 2, 3},
                                          {0, 1, 4},
                                          {1, 2, 3, 6},
                                          {1, 4, 6},
                                          {4, 5, 6, 7},
                                          {7, 8}};

static const VertexNumbersList MAX_CLIQUES = {CLIQUES[0], CLIQUES[2], CLIQUES[4]};

TEST(bron1_null) {
  SimpleGraph g;
  Bron1 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 0);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(b.total(), 0);
  UNITTEST_ASSERT_EQUAL(b.number(), 0);
  UNITTEST_ASSERT_TRUE(b.cliques().empty());
}

TEST(bron2_null) {
  SimpleGraph g;
  Bron2 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 0);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(b.total(), 0);
  UNITTEST_ASSERT_EQUAL(b.number(), 0);
  UNITTEST_ASSERT_TRUE(b.cliques().empty());
}

TEST(bron1_trivial) {
  SimpleGraph g(1);
  Bron1 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 2);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b.total(), 1);
  UNITTEST_ASSERT_EQUAL(b.number(), 1);
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), 1);
  VertexNumbers expected = {0};
  UNITTEST_ASSERT_EQUAL(b.cliques()[0].size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(b.cliques()[0], expected);
}

TEST(bron2_trivial) {
  SimpleGraph g(1);
  Bron2 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 2);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b.total(), 1);
  UNITTEST_ASSERT_EQUAL(b.number(), 1);
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), 1);
  VertexNumbers expected = {0};
  UNITTEST_ASSERT_EQUAL(b.cliques()[0].size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(b.cliques()[0], expected);
}

TEST(bron1_empty) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Bron1 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 11);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b.total(), 10);
  UNITTEST_ASSERT_EQUAL(b.number(), 1);
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), ORDER);
  for (VertexNumbersList::size_type ic = 0; ic < ORDER; ++ic) {
    const VertexNumbers &c = b.cliques()[ic];
    VertexNumbers expected = {ic};
    UNITTEST_ASSERT_EQUAL(c.size(), expected.size());
    UNITTEST_ASSERT_EQUAL_CONTAINERS(c, expected);
  }
}

TEST(bron2_empty) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Bron2 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 11);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b.total(), 10);
  UNITTEST_ASSERT_EQUAL(b.number(), 1);
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), ORDER);
  VertexNumbersList cliques = b.cliques();
  std::sort(cliques.begin(), cliques.end());
  for (VertexNumbersList::size_type ic = 0; ic < ORDER; ++ic) {
    const VertexNumbers &c = cliques[ic];
    VertexNumbers expected = {ic};
    UNITTEST_ASSERT_EQUAL(c.size(), expected.size());
    UNITTEST_ASSERT_EQUAL_CONTAINERS(c, expected);
  }
}

TEST(bron1_complete) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Bron1 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b.total(), 1);
  UNITTEST_ASSERT_EQUAL(b.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), 1);
  VertexNumbers expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.insert(iv);
  UNITTEST_ASSERT_EQUAL(b.cliques()[0].size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(b.cliques()[0], expected);
}

TEST(bron2_complete) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Bron2 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b.total(), 1);
  UNITTEST_ASSERT_EQUAL(b.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), 1);
  VertexNumbers expected;
  for (VertexNumber iv = 0; iv < ORDER; ++iv) expected.insert(iv);
  UNITTEST_ASSERT_EQUAL(b.cliques()[0].size(), expected.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(b.cliques()[0], expected);
}

TEST(bron1_sample) {
  SimpleGraph g(VERTICES, EDGES);
  Bron1 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 24);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b.number(), MAX_CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), CLIQUES.size());
  for (VertexNumbersList::size_type ic = 0; ic < CLIQUES.size(); ++ic) {
    UNITTEST_ASSERT_EQUAL_CONTAINERS(b.cliques()[ic], CLIQUES[ic]);
  }
}

TEST(bron2_sample) {
  SimpleGraph g(VERTICES, EDGES);
  Bron2 b(g);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 17);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b.number(), MAX_CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), CLIQUES.size());
  VertexNumbersList cliques = b.cliques();
  std::sort(cliques.begin(), cliques.end());
  for (VertexNumbersList::size_type ic = 0; ic < CLIQUES.size(); ++ic) {
    UNITTEST_ASSERT_EQUAL_CONTAINERS(cliques[ic], CLIQUES[ic]);
  }
}

TEST(bron1_max_only) {
  SimpleGraph g(VERTICES, EDGES);
  Bron1 b(g, Bron::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 24);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b.number(), MAX_CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), MAX_CLIQUES.size());
  for (VertexNumbersList::size_type ic = 0; ic < MAX_CLIQUES.size(); ++ic) {
    UNITTEST_ASSERT_EQUAL_CONTAINERS(b.cliques()[ic], MAX_CLIQUES[ic]);
  }
}

TEST(bron2_max_only) {
  SimpleGraph g(VERTICES, EDGES);
  Bron2 b(g, Bron::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 17);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b.number(), MAX_CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), MAX_CLIQUES.size());
  VertexNumbersList cliques = b.cliques();
  std::sort(cliques.begin(), cliques.end());
  for (VertexNumbersList::size_type ic = 0; ic < MAX_CLIQUES.size(); ++ic) {
    UNITTEST_ASSERT_EQUAL_CONTAINERS(cliques[ic], MAX_CLIQUES[ic]);
  }
}

TEST(bron1_first_max) {
  SimpleGraph g(VERTICES, EDGES);
  Bron1 b(g, Bron::MODE_FIRST_MAX);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 17);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b.total(), 1);
  UNITTEST_ASSERT_EQUAL(b.number(), MAX_CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(b.cliques()[0], MAX_CLIQUES[0]);
}

TEST(bron2_first_max) {
  SimpleGraph g(VERTICES, EDGES);
  Bron2 b(g, Bron::MODE_FIRST_MAX);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 13);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b.total(), 1);
  UNITTEST_ASSERT_EQUAL(b.number(), MAX_CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(b.cliques()[0], MAX_CLIQUES.back());
}

TEST(bron1_no_save) {
  SimpleGraph g(VERTICES, EDGES);
  Bron1 b(g, Bron::MODE_ALL, false);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 24);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b.number(), MAX_CLIQUES[0].size());
  UNITTEST_ASSERT_TRUE(b.cliques().empty())
}

TEST(bron2_no_save) {
  SimpleGraph g(VERTICES, EDGES);
  Bron2 b(g, Bron::MODE_ALL, false);
  UNITTEST_ASSERT_TRUE(b.execute());
  UNITTEST_ASSERT_EQUAL(b.calls(), 17);
  UNITTEST_ASSERT_EQUAL(b.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b.number(), MAX_CLIQUES[0].size());
  UNITTEST_ASSERT_TRUE(b.cliques().empty())
}
