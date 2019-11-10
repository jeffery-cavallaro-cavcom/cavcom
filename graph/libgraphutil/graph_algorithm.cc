#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    GraphAlgorithm::GraphAlgorithm(const SimpleGraph &graph) : graph_(graph), steps_(0), calls_(0) {}

    GraphAlgorithm::Duration GraphAlgorithm::duration() const {
      Time tf = (completed() ? Clock::now() : end_);
      return (tf - start_);
    }

    bool GraphAlgorithm::started(void) const {
      return (start_ != Time());
    }

    bool GraphAlgorithm::completed(void) const {
      return (end_ != Time());
    }

    bool GraphAlgorithm::execute() {
      // Initialize the context.
      start_ = Time();
      end_ = start_;
      steps_ = 0;
      calls_ = 0;

      // Execute the algorithm.
      start_ = Clock::now();
      bool success = run();
      end_ = Clock::now();

      return success;
    }

  }  // namespace graph
}  // namespace cavcom
