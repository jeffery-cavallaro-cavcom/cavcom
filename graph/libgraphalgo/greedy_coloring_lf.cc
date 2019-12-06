#include <algorithm>
#include <vector>

#include "greedy_coloring_lf.h"

namespace cavcom {
  namespace graph {

    GreedyColoringLF::GreedyColoringLF(const SimpleGraph &graph) : VertexColoringAlgorithm(graph) {}

    bool GreedyColoringLF::run() {
      VertexColoringAlgorithm::run();

      // Guard against an empty graph.
      VertexNumber n = graph().order();
      if (n <= 0) return true;

      // Sort the vertices by non-increasing degree.
      using VertexNumberList = std::vector<VertexNumber>;
      VertexNumberList vertices;
      for (VertexNumber iv = 0; iv < n; ++iv) vertices.push_back(iv);
      std::sort(vertices.begin(), vertices.end(),
                [&](VertexNumber iv, VertexNumber jv){ return graph().degree(iv) > graph().degree(jv); });

      // Keep track of all color assignments here.
      using ColorAssignments = std::vector<Color>;
      ColorAssignments colors(n, NOCOLOR);

      // Color the vertices, largest first.
      for (VertexNumber iv = 0; iv < n; ++iv) {
        VertexNumber next = vertices[iv];

        // Determine which colors are used by all adjacent vertices that have already been colored.
        std::vector<bool> used(n);
        for (VertexNumber jv = 0; jv < iv; ++jv) {
          add_step();
          VertexNumber prev = vertices[jv];
          if (graph().adjacent(next, prev)) used[colors[prev]] = true;
        }

        // Use the first unused color.
        Color ic = 0;
        for (ic = 0; used[ic]; ++ic) { add_step(); }
        colors[next] = ic;
      }

      // Extract the coloring.
      number_ = *std::max_element(colors.cbegin(), colors.cend()) + 1;
      coloring_.resize(number_);
      for (VertexNumber iv = 0; iv < n; ++iv) {
        coloring_[colors[iv]].insert(iv);
      }

      return true;
    }

  }  // namespace graph
}  // namespace cavcom
