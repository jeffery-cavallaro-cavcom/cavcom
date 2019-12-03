#include "bron.h"

namespace cavcom {
  namespace graph {

    Bron::Bron(const SimpleGraph &graph, int mode, bool save) : CliqueAlgorithm(graph, mode, save) {}

    bool Bron::run() {
      // Reset the context.
      CliqueAlgorithm::run();
      current_.clear();

      // Guard against a null graph.
      VertexNumber n = graph().order();
      if (n <= 0) return true;

      // At first, all vertices are candidates and nothing has been previously used.
      VertexNumberList candidates, used;
      candidates.reserve(n);
      VertexNumber iv = n;
      while (iv > 0) candidates.push_back(--iv);

      // Run the algorithm.
      return extend(&candidates, &used);
    }

  }  // namespace graph
}  // namespace cavcom
