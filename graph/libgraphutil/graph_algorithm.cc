#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    GraphAlgorithm::GraphAlgorithm(const SimpleGraph &graph)
      : graph_(graph), steps_(0), calls_(0), depth_(0), maxdepth_(0) {}

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
      depth_ = 0;
      maxdepth_ = 0;

      // Execute the algorithm.
      start_ = Clock::now();
      bool success = run();
      end_ = Clock::now();

      return success;
    }

    void GraphAlgorithm::add_step(void) {
      ++steps_;
    }

    void GraphAlgorithm::add_call(void) {
      ++calls_;
      ++depth_;
      if (depth_ > maxdepth_) maxdepth_ = depth_;
    }

    void GraphAlgorithm::done_call(void) {
      if (depth_ > 0) --depth_;
    }

  }  // namespace graph
}  // namespace cavcom
