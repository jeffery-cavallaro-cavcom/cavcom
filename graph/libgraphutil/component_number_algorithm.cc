#include "component_number_algorithm.h"

namespace cavcom {
  namespace graph {

    ComponentNumberAlgorithm::ComponentNumberAlgorithm(const SimpleGraph &graph) : GraphAlgorithm(graph), number_(0) {}

    bool ComponentNumberAlgorithm::run() {
      GraphAlgorithm::run();
      number_ = 0;
      return true;
    }

  }  // namespace graph
}  // namespace cavcom
