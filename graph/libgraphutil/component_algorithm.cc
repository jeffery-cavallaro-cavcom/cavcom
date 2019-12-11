#include "component_algorithm.h"

namespace cavcom {
  namespace graph {

    ComponentAlgorithm::ComponentAlgorithm(const SimpleGraph &graph, bool save)
      : ComponentNumberAlgorithm(graph), save_(save) {}

    bool ComponentAlgorithm::add_component(const VertexNumbers &next) {
      if (!found(next)) return false;
      if (save_) components_.push_back(next);
      ++number_;
      return true;
    }

    bool ComponentAlgorithm::run() {
      ComponentNumberAlgorithm::run();
      components_.clear();
      return true;
    }

  }  // namespace graph
}  // namespace cavcom
