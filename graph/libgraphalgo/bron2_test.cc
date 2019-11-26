// Uses the Bron-2 algorithm to find all maximal cliques in a graph.

#include <algorithm>

#include <libunittest/all.hpp>

#include "bron2.h"

using namespace cavcom::graph;

// A derived Bron-2 class that includes a clique hook to record each found clique.
class Bron2All : public Bron2 {
 public:
  Bron2All(const SimpleGraph &graph, bool fail = false) : Bron2(graph, Bron2::MODE_MAX), fail_(fail) {}

  const Cliques &all(void) const { return all_; }

 protected:
  virtual bool found(const Clique &clique) {
    all_.push_back(clique);
    return (!fail_ || (all_.size() < 2));
  }

 private:
  Cliques all_;
  bool fail_;
};

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

static const Bron2::Cliques CLIQUES = {{0, 1, 2, 3},
                                       {0, 1, 4},
                                       {1, 2, 3, 6},
                                       {1, 4, 6},
                                       {4, 5, 6, 7},
                                       {7, 8}};

TEST(null_all) {
  SimpleGraph g;
  Bron2 b2(g);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 0);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(b2.total(), 0);
  UNITTEST_ASSERT_EQUAL(b2.number(), 0);
  UNITTEST_ASSERT_TRUE(b2.cliques().empty());
}

TEST(null_max) {
  SimpleGraph g;
  Bron2 b2(g, Bron2::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 0);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(b2.total(), 0);
  UNITTEST_ASSERT_EQUAL(b2.number(), 0);
  UNITTEST_ASSERT_TRUE(b2.cliques().empty());
}

TEST(null_max_only) {
  SimpleGraph g;
  Bron2 b2(g, Bron2::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 0);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 0);
  UNITTEST_ASSERT_EQUAL(b2.total(), 0);
  UNITTEST_ASSERT_EQUAL(b2.number(), 0);
  UNITTEST_ASSERT_TRUE(b2.cliques().empty());
}

TEST(trivial_all) {
  SimpleGraph g(1);
  Bron2 b2(g);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 2);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b2.total(), 1);
  UNITTEST_ASSERT_EQUAL(b2.number(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0][0], 0);
}

TEST(trivial_max) {
  SimpleGraph g(1);
  Bron2 b2(g, Bron2::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 2);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b2.total(), 1);
  UNITTEST_ASSERT_EQUAL(b2.number(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0][0], 0);
}

TEST(trivial_max_only) {
  SimpleGraph g(1);
  Bron2 b2(g, Bron2::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 2);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b2.total(), 1);
  UNITTEST_ASSERT_EQUAL(b2.number(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0][0], 0);
}

TEST(empty_all) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Bron2 b2(g);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 11);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b2.total(), 10);
  UNITTEST_ASSERT_EQUAL(b2.number(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), ORDER);
  std::vector<bool> found(ORDER);
  for (Bron2::Cliques::size_type ic = 0; ic < ORDER; ++ic) {
    const Bron2::Clique &c = b2.cliques()[ic];
    UNITTEST_ASSERT_EQUAL(c.size(), 1);
    VertexNumber v = c[0];
    UNITTEST_ASSERT_FALSE(found[v]);
    found[v] = true;
  }
}

TEST(empty_max) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Bron2 b2(g, Bron2::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 11);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b2.total(), 10);
  UNITTEST_ASSERT_EQUAL(b2.number(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0][0], 9);
}

TEST(empty_max_only) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  Bron2 b2(g, Bron2::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 11);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 2);
  UNITTEST_ASSERT_EQUAL(b2.total(), 1);
  UNITTEST_ASSERT_EQUAL(b2.number(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0].size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0][0], 9);
}

TEST(complete_all) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Bron2 b2(g);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b2.total(), 1);
  UNITTEST_ASSERT_EQUAL(b2.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0].size(), ORDER);
  Bron2::Clique sorted(b2.cliques()[0]);
  std::sort(sorted.begin(), sorted.end());
  for (Bron2::Clique::size_type ic = 0; ic < ORDER; ++ic) {
    UNITTEST_ASSERT_EQUAL(sorted[ic], ic);
  }
}

TEST(complete_max) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Bron2 b2(g, Bron2::MODE_MAX);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b2.total(), 1);
  UNITTEST_ASSERT_EQUAL(b2.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0].size(), ORDER);
  Bron2::Clique sorted(b2.cliques()[0]);
  std::sort(sorted.begin(), sorted.end());
  for (Bron2::Clique::size_type ic = 0; ic < ORDER; ++ic) {
    UNITTEST_ASSERT_EQUAL(sorted[ic], ic);
  }
}

TEST(complete_max_only) {
  const VertexNumber ORDER = 10;
  SimpleGraph g(ORDER);
  g.make_complete();
  Bron2 b2(g, Bron2::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), ORDER + 1);
  UNITTEST_ASSERT_EQUAL(b2.total(), 1);
  UNITTEST_ASSERT_EQUAL(b2.number(), ORDER);
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL(b2.cliques()[0].size(), ORDER);
  Bron2::Clique sorted(b2.cliques()[0]);
  std::sort(sorted.begin(), sorted.end());
  for (Bron2::Clique::size_type ic = 0; ic < ORDER; ++ic) {
    UNITTEST_ASSERT_EQUAL(sorted[ic], ic);
  }
}

TEST(sample_all) {
  SimpleGraph g(VERTICES, EDGES);
  Bron2 b2(g);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 17);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b2.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b2.number(), CLIQUES[0].size());
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), CLIQUES.size());
  Bron2::Cliques sorted;
  for (Bron2::Cliques::size_type ic = 0; ic < b2.cliques().size(); ++ic) {
    sorted.push_back(b2.cliques()[ic]);
    std::sort(sorted.back().begin(), sorted.back().end());
  }
  for (Bron2::Cliques::size_type ic = 0; ic < CLIQUES.size(); ++ic) {
    UNITTEST_ASSERT_IN_CONTAINER(CLIQUES[ic], sorted);
  }
}

TEST(sample_max) {
  SimpleGraph g(VERTICES, EDGES);
  Bron2All b2(g);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 17);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b2.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(b2.number(), CLIQUES[4].size());
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  Bron2::Clique sorted(b2.cliques()[0]);
  std::sort(sorted.begin(), sorted.end());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(sorted, CLIQUES[4]);
  UNITTEST_ASSERT_EQUAL(b2.all().size(), CLIQUES.size());
  for (Bron2::Cliques::size_type ic = 0; ic < CLIQUES.size(); ++ic) {
    sorted = b2.all()[ic];
    std::sort(sorted.begin(), sorted.end());
    UNITTEST_ASSERT_IN_CONTAINER(sorted, CLIQUES);
  }
}

TEST(sample_max_only) {
  SimpleGraph g(VERTICES, EDGES);
  Bron2 b2(g, Bron2::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 13);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b2.total(), 1);
  UNITTEST_ASSERT_EQUAL(b2.number(), CLIQUES[4].size());
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  Bron2::Clique sorted(b2.cliques()[0]);
  std::sort(sorted.begin(), sorted.end());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(sorted, CLIQUES[4]);
}

TEST(early_stop) {
  SimpleGraph g(VERTICES, EDGES);
  Bron2All b2(g, true);
  UNITTEST_ASSERT_FALSE(b2.execute());
  UNITTEST_ASSERT_EQUAL(b2.calls(), 6);
  UNITTEST_ASSERT_EQUAL(b2.maxdepth(), 5);
  UNITTEST_ASSERT_EQUAL(b2.total(), 2);
  UNITTEST_ASSERT_EQUAL(b2.number(), CLIQUES[4].size());
  UNITTEST_ASSERT_EQUAL(b2.cliques().size(), 1);
  Bron2::Clique sorted(b2.cliques()[0]);
  std::sort(sorted.begin(), sorted.end());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(sorted, CLIQUES[4]);
  UNITTEST_ASSERT_EQUAL(b2.all().size(), 2);
  sorted = b2.all()[0];
  std::sort(sorted.begin(), sorted.end());
  UNITTEST_ASSERT_IN_CONTAINER(sorted, CLIQUES);
  sorted = b2.all()[1];
  std::sort(sorted.begin(), sorted.end());
  UNITTEST_ASSERT_IN_CONTAINER(sorted, CLIQUES);
}
