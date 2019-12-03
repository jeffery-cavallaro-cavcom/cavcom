#include "clique_number_algorithm.h"

namespace cavcom {
  namespace graph {

    CliqueNumberAlgorithm::CliqueNumberAlgorithm(const SimpleGraph &graph) : GraphAlgorithm(graph), number_(0) {}

    bool CliqueNumberAlgorithm::run() {
      GraphAlgorithm::run();
      number_ = 0;
      return true;
    }

  }  // namespace graph
}  // namespace cavcom
