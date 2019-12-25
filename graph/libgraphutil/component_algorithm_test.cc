// Executes a dummy component algorithm.

#include <libunittest/all.hpp>

#include "component_algorithm.h"

using namespace cavcom::graph;

// A dummy component algorithm.
static const VertexNumbersList COMPONENTS = {{0},
                                             {1, 2},
                                             {4, 8},
                                             {3, 6, 7, 9},
                                             {5}};

class DummyAlgorithm : public ComponentAlgorithm {
 public:
  DummyAlgorithm(const SimpleGraph &graph, bool save, bool fail)
    : ComponentAlgorithm(graph, save), fail_(fail), status_(true) {}

 private:
  bool fail_;
  bool status_;

  virtual bool run() {
    ComponentAlgorithm::run();
    status_ = true;
    for (VertexNumbersList::size_type ic = 0; ic < COMPONENTS.size(); ++ic) {
      if (fail_ && (ic >= 2)) status_ = false;
      add_component(COMPONENTS[ic]);
    }
    return status_;
  }

  virtual bool found(const VertexNumbers &next) {
    add_step();
    if (save() && (next != COMPONENTS[number_])) status_ = false;
    return status_;
  }
};

TEST(save_all) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, true, false);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.components().empty());
  UNITTEST_ASSERT_TRUE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_EQUAL(ca.components().size(), COMPONENTS.size());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ca.components(), COMPONENTS);
  UNITTEST_ASSERT_EQUAL(ca.number(), COMPONENTS.size());
}

TEST(save_none) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, false, false);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.components().empty());
  UNITTEST_ASSERT_FALSE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_TRUE(ca.components().empty());
  UNITTEST_ASSERT_EQUAL(ca.number(), COMPONENTS.size());
}

TEST(failed_run) {
  SimpleGraph graph(0);
  DummyAlgorithm ca(graph, true, true);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(ca.components().empty());
  UNITTEST_ASSERT_TRUE(ca.save());
  UNITTEST_ASSERT_EQUAL(ca.number(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_FALSE(ca.execute());

  // Check the end state.
  UNITTEST_ASSERT_EQUAL(ca.components().size(), 2);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ca.components()[0], COMPONENTS[0]);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(ca.components()[1], COMPONENTS[1]);
  UNITTEST_ASSERT_EQUAL(ca.number(), 2);
}
