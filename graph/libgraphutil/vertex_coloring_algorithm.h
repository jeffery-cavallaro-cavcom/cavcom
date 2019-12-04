#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_VERTEX_COLORING_ALGORITHM_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_VERTEX_COLORING_ALGORITHM_H_

#include <vector>

#include "chromatic_number_algorithm.h"

namespace cavcom {
  namespace graph {

    // Any algorithm that constructs a proper or chromatic coloring for a graph.
    class VertexColoringAlgorithm : public ChromaticNumberAlgorithm {
     public:
      using Coloring = std::vector<VertexNumbers>;

      // Creates a new instance of a vertex coloring algorithm for the specified graph.
      explicit VertexColoringAlgorithm(const SimpleGraph &graph);

      // Returns the constructed proper or chromatic coloring.
      const Coloring &coloring(void) const { return coloring_; }

      // Applies the found coloring to the specified graph.
      void apply(Graph *graph);

     protected:
      // The constructed proper or chromatic coloring.
      Coloring coloring_;

      // Calls the base class method and resets the derived state.
      virtual bool run();
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_VERTEX_COLORING_ALGORITHM_H_
