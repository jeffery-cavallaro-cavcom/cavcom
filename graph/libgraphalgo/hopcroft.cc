#include "hopcroft.h"

namespace cavcom {
  namespace graph {

    Hopcroft::Hopcroft(const SimpleGraph &graph, bool save)
      : ComponentAlgorithm(graph, save), used_(graph.order()), nunused_(graph.order()) {}

    bool Hopcroft::run() {
      // Reset the base context.
      ComponentAlgorithm::run();

      // Assign all vertices.
      bool status = true;
      while (status && (nunused_ > 0)) {
        VertexNumbers next;
        VertexNumber iv = 0;
        while (used_[iv]) ++iv;
        add_to_component(iv, &next);
        status = add_component(next);
      }

      return status;
    }

    void Hopcroft::add_to_component(VertexNumber iv, VertexNumbers *next) {
      add_call();

      // Add the next vertex if not already assigned.
      next->insert(iv);
      used_[iv] = true;
      --nunused_;

      // Assign all of its unused adjacent vertices.
      VertexNumber n = graph().order();
      for (VertexNumber jv = 0; jv < n; ++jv) {
        if ((!used_[jv]) && graph().adjacent(iv, jv)) add_to_component(jv, next);
      }

      done_call();
    }

  }  // namespace graph
}  // namespace cavcom
