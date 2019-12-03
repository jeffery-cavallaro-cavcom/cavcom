// Executes a dummy clique algorithm.

#include <libunittest/all.hpp>

#include "clique_algorithm.h"

using namespace cavcom::graph;

// A dummy clique algorithm.
static const CliqueAlgorithm::Cliques CLIQUES = {{0},
                                                 {0, 1},
                                                 {1, 3, 4, 5, 9},
                                                 {2, 5, 6},
                                                 {1, 2, 3, 4, 5},
                                                 {4, 5, 8, 9},
                                                 {5}};

static const CliqueAlgorithm::Cliques MAX_CLIQUES{CLIQUES[2], CLIQUES[4]};

static const CliqueAlgorithm::Cliques PARTIAL_CLIQUES{CLIQUES[0], CLIQUES[1]};

class DummyAlgorithm : public CliqueAlgorithm {
 public:
  DummyAlgorithm(const SimpleGraph &graph, int mode, bool save, bool fail)
    : CliqueAlgorithm(graph, mode, save), fail_(fail), status_(true) {}

 private:
  bool fail_;
  bool status_;

  virtual bool run() {
    CliqueAlgorithm::run();
    status_ = true;
    for (CliqueAlgorithm::Cliques::size_type ic = 0; ic < CLIQUES.size(); ++ic) {
      if (fail_ && (ic >= 2)) status_ = false;
      add_clique(CLIQUES[ic]);
    }
    return status_;
  }

  virtual bool found(const VertexNumbers &next) {
    add_step();
    return status_;
  }
};

TEST(save_all) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, DummyAlgorithm::MODE_ALL, true, false);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.mode(), DummyAlgorithm::MODE_ALL);
  UNITTEST_ASSERT_TRUE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.total(), 0);
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_EQUAL(ca.cliques().size(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ca.cliques(), CLIQUES);
  UNITTEST_ASSERT_EQUAL(ca.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(ca.number(), 5);
}

TEST(count_all) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, DummyAlgorithm::MODE_ALL, false, false);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.mode(), DummyAlgorithm::MODE_ALL);
  UNITTEST_ASSERT_FALSE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.total(), 0);
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(ca.number(), 5);
}

TEST(save_max) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, DummyAlgorithm::MODE_MAX_ONLY, true, false);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.mode(), DummyAlgorithm::MODE_MAX_ONLY);
  UNITTEST_ASSERT_TRUE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.total(), 0);
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_EQUAL(ca.cliques().size(), 2);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ca.cliques(), MAX_CLIQUES);
  UNITTEST_ASSERT_EQUAL(ca.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(ca.number(), 5);
}

TEST(count_max) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, DummyAlgorithm::MODE_MAX_ONLY, false, false);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.mode(), DummyAlgorithm::MODE_MAX_ONLY);
  UNITTEST_ASSERT_FALSE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.total(), 0);
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(ca.number(), 5);
}

TEST(save_first) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, DummyAlgorithm::MODE_FIRST_MAX, true, false);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.mode(), DummyAlgorithm::MODE_FIRST_MAX);
  UNITTEST_ASSERT_TRUE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.total(), 0);
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_EQUAL(ca.cliques().size(), 1);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ca.cliques()[0], MAX_CLIQUES[0]);
  UNITTEST_ASSERT_EQUAL(ca.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(ca.number(), 5);
}

TEST(count_first) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, DummyAlgorithm::MODE_FIRST_MAX, false, false);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.mode(), DummyAlgorithm::MODE_FIRST_MAX);
  UNITTEST_ASSERT_FALSE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.total(), 0);
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.total(), CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(ca.number(), 5);
}

TEST(failed_run) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, DummyAlgorithm::MODE_ALL, true, true);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.cliques().empty());
  UNITTEST_ASSERT_EQUAL(ca.mode(), DummyAlgorithm::MODE_ALL);
  UNITTEST_ASSERT_TRUE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.total(), 0);
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_FALSE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_EQUAL(ca.cliques().size(), PARTIAL_CLIQUES.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ca.cliques(), PARTIAL_CLIQUES);
  UNITTEST_ASSERT_EQUAL(ca.total(), PARTIAL_CLIQUES.size());
  UNITTEST_ASSERT_EQUAL(ca.number(), 2);
}
