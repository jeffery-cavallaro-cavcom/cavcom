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
      if ((!status_) && (i >= 1)) break;
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
  DummyAlgorithm ga(graph);

  // Check the initialized state.
  UNITTEST_ASSERT_TRUE(is_epoch(ga.start()));
  UNITTEST_ASSERT_TRUE(is_epoch(ga.end()));
  UNITTEST_ASSERT_FALSE(ga.started());
  UNITTEST_ASSERT_FALSE(ga.completed());
  std::chrono::duration<double> d = ga.duration();
  UNITTEST_ASSERT_EQUAL(d.count(), 0);
  UNITTEST_ASSERT_EQUAL(ga.steps(), 0);
  UNITTEST_ASSERT_EQUAL(ga.calls(), 0);
  UNITTEST_ASSERT_EQUAL(ga.depth(), 0);
  UNITTEST_ASSERT_EQUAL(ga.maxdepth(), 0);

  // Run the algorithm.
  UNITTEST_ASSERT_TRUE(ga.execute());

  // Check the end state.
  UNITTEST_ASSERT_FALSE(is_epoch(ga.start()));
  UNITTEST_ASSERT_TRUE(ga.start() < ga.end());
  UNITTEST_ASSERT_TRUE(ga.started());
  UNITTEST_ASSERT_TRUE(ga.completed());
  d = ga.duration();
  UNITTEST_ASSERT_GREATER_EQUAL(d.count(), 3);
  UNITTEST_ASSERT_EQUAL(ga.steps(), 6);
  UNITTEST_ASSERT_EQUAL(ga.calls(), 3);
  UNITTEST_ASSERT_EQUAL(ga.depth(), 0);
  UNITTEST_ASSERT_EQUAL(ga.maxdepth(), 1);
}

TEST(failed_run) {
  SimpleGraph graph(5);
  DummyAlgorithm ga(graph, false);
  UNITTEST_ASSERT_FALSE(ga.execute());
  UNITTEST_ASSERT_FALSE(is_epoch(ga.start()));
  UNITTEST_ASSERT_TRUE(ga.start() < ga.end());
  UNITTEST_ASSERT_TRUE(ga.started());
  UNITTEST_ASSERT_TRUE(ga.completed());
  std::chrono::duration<double> d = ga.duration();
  UNITTEST_ASSERT_LESSER(d.count(), 3);
  UNITTEST_ASSERT_EQUAL(ga.steps(), 4);
  UNITTEST_ASSERT_EQUAL(ga.calls(), 2);
  UNITTEST_ASSERT_EQUAL(ga.depth(), 0);
  UNITTEST_ASSERT_EQUAL(ga.maxdepth(), 1);
}
