#include "chromatic_number_algorithm.h"

namespace cavcom {
  namespace graph {

    ChromaticNumberAlgorithm::ChromaticNumberAlgorithm(const SimpleGraph &graph) : GraphAlgorithm(graph), number_(0) {}

    bool ChromaticNumberAlgorithm::run() {
      GraphAlgorithm::run();
      number_ = 0;
      return true;
    }

  }  // namespace graph
}  // namespace cavcom
