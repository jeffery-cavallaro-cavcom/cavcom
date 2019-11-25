#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // The Christofides algorithm for finding the chromatic number of a graph.
    class Christofides : public GraphAlgorithm {
     public:
      Christofides(const SimpleGraph &graph);

     private:
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_
