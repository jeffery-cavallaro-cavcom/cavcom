#include "greedy_coloring.h"

namespace cavcom {
  namespace graph {

    GreedyColoring::GreedyColoring(const SimpleGraph &graph, bool interchange)
      : VertexColoringAlgorithm(graph),
        sorted_(graph.order()), colors_(graph.order(), NOCOLOR), interchange_(interchange) {}

    bool GreedyColoring::run() {
      // Initialize the base and derived context.
      VertexColoringAlgorithm::run();

      // Guard against an empty graph.
      VertexNumber n = graph().order();
      if (n <= 0) return true;

      // Sort the vertices into the desired coloring order.
      for (VertexNumber iv = 0; iv < n; ++iv) sorted_[iv] = iv;
      sort();

      // Color the vertices, largest first.
      for (VertexNumber iv = 0; iv < n; ++iv) {
        VertexNumber next = sorted_[iv];

        // Determine which colors are used by all adjacent vertices that have already been colored.
        std::vector<bool> used(iv + 1);
        for (VertexNumber jv = 0; jv < iv; ++jv) {
          add_step();
          VertexNumber prev = sorted_[jv];
          if (graph().adjacent(next, prev)) used[colors_[prev]] = true;
        }

        // Use the first unused color.
        Color ic = 0;
        for (ic = 0; used[ic]; ++ic) { add_step(); }
        colors_[next] = ic;
      }

      // Extract the coloring.
      number_ = *std::max_element(colors_.cbegin(), colors_.cend()) + 1;
      coloring_.resize(number_);
      for (VertexNumber iv = 0; iv < n; ++iv) {
        coloring_[colors_[iv]].insert(iv);
      }

      return true;
    }

    void GreedyColoring::sort(void) {
      std::sort(sorted_.begin(), sorted_.end(),
                [&](VertexNumber iv, VertexNumber jv){ return graph().degree(iv) > graph().degree(jv); });
    }

  }  // namespace graph
}  // namespace cavcom
