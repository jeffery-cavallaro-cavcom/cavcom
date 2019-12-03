#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_CLIQUE_NUMBER_ALGORITHM_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_CLIQUE_NUMBER_ALGORITHM_H_

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // Any algorithm that finds or estimates the clique number for a graph.
    class CliqueNumberAlgorithm : public GraphAlgorithm {
     public:
      // Creates a new clique number algorithm for the specified graph.  The graph must remain valid during the
      // lifetime of the algorithm.
      explicit CliqueNumberAlgorithm(const SimpleGraph &graph);

      // Returns the found or estimated clique number.
      VertexNumber number() const { return number_; }

     protected:
      VertexNumber number_;

      // Calls the base class method and resets the clique number.
      virtual bool run();
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_CLIQUE_NUMBER_ALGORITHM_H_
