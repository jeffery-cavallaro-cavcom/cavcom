#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_CHROMATIC_WANG_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_CHROMATIC_WANG_H_

#include <vector>

#include "vertex_coloring_algorithm.h"

namespace cavcom {
  namespace graph {

    // The Wang algorithm is an improvement of the Christofides algorithm.  It only uses the smallest subset of
    // MISs of a subgraph that contain a particular vertex.  It also searches depth-first, looking for the leaf
    // state at the shallowest level.
    class ChromaticWang : public VertexColoringAlgorithm {
     public:
      // Create a new Wang algorithm instance for the specified graph.
      ChromaticWang(const SimpleGraph &graph);

     private:
      using ColoringByIDs = std::vector<VertexIDs>;

      // A MIS is a graph is a clique in its complement.
      SimpleGraph complement_;

      // Calls the base class method and then runs the algorithm.
      virtual bool run();

      // Generates the MISs for the next state, prunes them, and recursively calls itself to look for the
      // smallest path to completion.
      void next_states(const ColoringByIDs &coloring, VertexIDs vertices);

      // Registers the specified coloring (by vertex ID) as the current chromatic coloring.
      void chromatic(const ColoringByIDs &coloring);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_CHROMATIC_WANG_H_
