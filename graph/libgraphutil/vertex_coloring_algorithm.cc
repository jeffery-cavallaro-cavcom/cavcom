#include <algorithm>

#include "vertex_coloring_algorithm.h"

namespace cavcom {
  namespace graph {

    VertexColoringAlgorithm::VertexColoringAlgorithm(const SimpleGraph &graph) : ChromaticNumberAlgorithm(graph) {}

    bool VertexColoringAlgorithm::run() {
      ChromaticNumberAlgorithm::run();
      coloring_.clear();
      return true;
    }

    void VertexColoringAlgorithm::apply(Graph *graph) {
      VertexNumber n = graph->order();
      for (VertexNumber iv = 0; iv < n; ++iv) graph->vertex(iv).color(NOCOLOR);

      Coloring::size_type nc = coloring_.size();
      for (Coloring::size_type ic = 0; ic < nc; ++ic) {
        Color color = ic + 1;
        const VertexNumbers &c = coloring_[ic];
        for_each(c.cbegin(), c.cend(), [&](VertexNumber iv){ graph->vertex(iv).color(color); });
      }
    }

  }  // namespace graph
}  // namespace cavcom
