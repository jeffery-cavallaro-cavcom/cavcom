#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_CLIQUE_ALGORITHM_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_CLIQUE_ALGORITHM_H_

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // Any algorithm that attempts to locate a clique (a complete subgraph) in a graph.
    class CliqueAlgorithm : public GraphAlgorithm {
     public:
      // Creates a new clique algorithm instance for an algorithm to be performed on the specified graph.  The
      // graph must remain valid during the lifetime of the algorithm.
      explicit CliqueAlgorithm(const SimpleGraph &graph) : GraphAlgorithm(graph) {}

      // Returns the found clique.
      const VertexNumbers &clique(void) const { return clique_; }

     protected:
      // Calls the base class method and resets the found clique.
      virtual bool run() {
        if (!GraphAlgorithm::run()) return false;
        clique_.clear();
        return true;
      }

     protected:
      VertexNumbers clique_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_CLIQUE_ALGORITHM_H_
