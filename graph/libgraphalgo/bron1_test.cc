// Uses the Bron-1 algorithm to find all maximal cliques in a graph.

#include <libunittest/all.hpp>

#include "bron1.h"

using namespace cavcom::graph;

// A derived Bron-1 class that includes a clique hook to record each found clique.
class Bron1All : public Bron1 {
 public:
  Bron1All(const SimpleGraph &graph) : Bron1(graph, Bron1::MODE_MAX) {}

  const Cliques &all(void) const { return all_; }

 protected:
  virtual void found(const Clique &clique) {
    all_.push_back(clique);
  }

 private:
  Cliques all_;
};

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
                                     {3, 6},
                                     {4, 5}, {4, 6}, {4, 7},
                                     {5, 6}, {5, 7},
                                     {6, 7}};

static const Bron1::Cliques CLIQUES = {{0, 1, 2, 3},
                                       {0, 1, 4},
                                       {1, 2, 3, 6},
                                       {1, 4, 6},
                                       {4, 5, 6, 7}};

TEST(null_all) {
  SimpleGraph g;
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 0);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(b1.total(), 0);
  UNITTEST_ASSERT_EQUAL(b1.number(), 0);
  UNITTEST_ASSERT_TRUE(b1.cliques().empty());
}

TEST(null_max) {
  SimpleGraph g;
  Bron1 b1(g, Bron1::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 0);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(b1.total(), 0);
  UNITTEST_ASSERT_EQUAL(b1.number(), 0);
  UNITTEST_ASSERT_TRUE(b1.cliques().empty());
}

TEST(null_max_only) {
  SimpleGraph g;
  Bron1 b1(g, Bron1::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 0);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(b1.total(), 0);
  UNITTEST_ASSERT_EQUAL(b1.number(), 0);
  UNITTEST_ASSERT_TRUE(b1.cliques().empty());
}

TEST(trivial_all) {
  SimpleGraph g(1);
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 2);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b1.total(), 1);
  UNITTEST_ASSERT_EQUAL(b1.number(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0][0], 0);
}

TEST(trivial_max) {
  SimpleGraph g(1);
  Bron1 b1(g, Bron1::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 2);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b1.total(), 1);
  UNITTEST_ASSERT_EQUAL(b1.number(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0][0], 0);
}

TEST(trivial_max_only) {
  SimpleGraph g(1);
  Bron1 b1(g, Bron1::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 2);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b1.total(), 1);
  UNITTEST_ASSERT_EQUAL(b1.number(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0][0], 0);
}

TEST(empty_all) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 11);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b1.total(), 10);
  UNITTEST_ASSERT_EQUAL(b1.number(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), ORDER);
  for (Bron1::Cliques::size_type ic = 0; ic < ORDER; ++ic) {
    const Bron1::Clique &c = b1.cliques()[ic];
    UNITTEST_ASSERT_EQUAL(c.size(), 1);
    UNITTEST_ASSERT_EQUAL(c[0], ic);
  }
}

TEST(empty_max) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Bron1 b1(g, Bron1::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 11);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b1.total(), 10);
  UNITTEST_ASSERT_EQUAL(b1.number(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0][0], 0);
}

TEST(empty_max_only) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Bron1 b1(g, Bron1::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 11);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b1.total(), 1);
  UNITTEST_ASSERT_EQUAL(b1.number(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0][0], 0);
}

TEST(complete_all) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b1.total(), 1);
  UNITTEST_ASSERT_EQUAL(b1.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), ORDER);
  for (Bron1::Clique::size_type ic = 0; ic < ORDER; ++ic) {
    UNITTEST_ASSERT_EQUAL(b1.cliques()[0][ic], ic);
  }
}

TEST(complete_max) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Bron1 b1(g, Bron1::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b1.total(), 1);
  UNITTEST_ASSERT_EQUAL(b1.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), ORDER);
  for (Bron1::Clique::size_type ic = 0; ic < ORDER; ++ic) {
    UNITTEST_ASSERT_EQUAL(b1.cliques()[0][ic], ic);
  }
}

TEST(complete_max_only) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Bron1 b1(g, Bron1::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b1.total(), 1);
  UNITTEST_ASSERT_EQUAL(b1.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b1.cliques()[0].size(), ORDER);
  for (Bron1::Clique::size_type ic = 0; ic < ORDER; ++ic) {
    UNITTEST_ASSERT_EQUAL(b1.cliques()[0][ic], ic);
  }
}

TEST(sample_all) {
  SimpleGraph g(VERTICES, EDGES);
  Bron1 b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 20);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b1.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b1.number(), CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), CLIQUES.size());
  for (Bron1::Cliques::size_type ic = 0; ic < CLIQUES.size(); ++ic) {
    UNITTEST_ASSERT_EQUAL_CONTAINERS(b1.cliques()[ic], CLIQUES[ic]);
  }
}

TEST(sample_max) {
  SimpleGraph g(VERTICES, EDGES);
  Bron1All b1(g);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 20);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b1.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b1.number(), CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(b1.cliques()[0], CLIQUES[0]);
  UNITTEST_ASSERT_EQUAL(b1.all().size(), CLIQUES.size());
  for (Bron1::Cliques::size_type ic = 0; ic < CLIQUES.size(); ++ic) {
    UNITTEST_ASSERT_EQUAL_CONTAINERS(b1.all()[ic], CLIQUES[ic]);
  }
}

TEST(sample_max_only) {
  SimpleGraph g(VERTICES, EDGES);
  Bron1 b1(g, Bron1::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b1.execute());
  UNITTEST_ASSERT_EQUAL(b1.calls(), 14);
  UNITTEST_ASSERT_EQUAL(b1.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b1.total(), 1);
  UNITTEST_ASSERT_EQUAL(b1.number(), CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b1.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(b1.cliques()[0], CLIQUES[0]);
}
