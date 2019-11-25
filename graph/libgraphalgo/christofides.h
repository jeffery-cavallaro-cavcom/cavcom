#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_

#include <vector>

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // The Christofides algorithm for finding the chromatic number of a graph.
    class Christofides : public GraphAlgorithm {
     public:
      using Coloring = std::vector<VertexNumbers>;

      Christofides(const SimpleGraph &graph);

      // Returns the first found chromatic coloring.
      const Coloring &chromatic(void) const { return chromatic_; }

      // Returns the chromatic number of the graph.
      Coloring::size_type number(void) const { return chromatic_.size(); }

     private:
      Coloring chromatic_;

      virtual bool run();

      friend class ChristofidesNode;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_
