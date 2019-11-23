// Executes a dummy algorithm.

#include <chrono>
#include <thread>

#include <libunittest/all.hpp>

#include "graph_algorithm.h"

using namespace cavcom::graph;

// A dummy algorithm.
class DummyAlgorithm : public GraphAlgorithm {
 public:
  DummyAlgorithm(const SimpleGraph &graph, bool status = true) : GraphAlgorithm(graph), status_(status) {}

 private:
  bool status_;

  virtual bool run() {
    for (int i = 0; i < 3; ++i) {
      add_step();
      method_call();
      add_step();
    }
    return status_;
  }

  void method_call(void) {
    add_call();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    done_call();
  }
};

static const DummyAlgorithm::Time epoch;
static bool is_epoch(const DummyAlgorithm::Time &t) { return (t == epoch); }

TEST(successful_run) {
  SimpleGraph graph(5);
  DummyAlgorithm algo(graph);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(is_epoch(algo.start()));
  UNITTEST_ASSERT_TRUE(is_epoch(algo.end()));
  UNITTEST_ASSERT_FALSE(algo.started());
  UNITTEST_ASSERT_FALSE(algo.completed());
  UNITTEST_ASSERT_EQUAL(algo.steps(), 0);
  UNITTEST_ASSERT_EQUAL(algo.calls(), 0);
  UNITTEST_ASSERT_EQUAL(algo.depth(), 0);
  UNITTEST_ASSERT_EQUAL(algo.maxdepth(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(algo.execute());

  // Check the end state.
  UNITTEST_ASSERT_FALSE(is_epoch(algo.start()));
  UNITTEST_ASSERT_TRUE(algo.start() < algo.end());
  UNITTEST_ASSERT_TRUE(algo.started());
  UNITTEST_ASSERT_TRUE(algo.completed());
  UNITTEST_ASSERT_EQUAL(algo.steps(), 6);
  UNITTEST_ASSERT_EQUAL(algo.calls(), 3);
  UNITTEST_ASSERT_EQUAL(algo.depth(), 0);
  UNITTEST_ASSERT_EQUAL(algo.maxdepth(), 1);
}
