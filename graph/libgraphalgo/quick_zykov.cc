#include <utility>

#include "quick_zykov.h"

namespace cavcom {
  namespace graph {

    QuickZykov::QuickZykov(const Graph &graph) : GraphAlgorithm(graph), k_(0) {}

    bool QuickZykov::run() {
      // Make a dynamic copy of the graph so that the algorithm can replace it with subgraphs.
      GraphPtr pg(new Graph(graph()));

      // Call the outer loop.  Note that the algorithm is always successful.
      outer_loop(&pg);

      return true;
    }

    void QuickZykov::outer_loop(GraphPtr *ppg) {
      Graph &g = **ppg;

      // Check for a null graph (n = 0).
      add_step();
      if (g.order() <= 0) {
        k_ = 0;
        return;
      }

      // Check for an empty graph (m = 0).
      add_step();
      if (g.size() <= 0) {
        k_ = 1;
        return;
      }

      add_step();
      k_ = 2;

      // Determine if the graph in the current state is k-colorable.  Is so, then done - the current value of k is
      // the chromatic number.  Otherwise, try the next value of k.  This is guaranteed to terminate since at the
      // very worst k will reach n.  Note that a subgraph may be returned if not k-colorable.
      while (!is_k_colorable(ppg)) {
        add_step();
        ++k_;
      }

      // Return the final graph.
      chromatic_ = std::move(*ppg);
    }

    bool QuickZykov::is_k_colorable(GraphPtr *psg) {
      add_call();
      return false;
    }

  }  // namespace graph
}  // namespace cavcom
