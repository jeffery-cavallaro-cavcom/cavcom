#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_CLIQUE_EDWARDS_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_CLIQUE_EDWARDS_H_

#include "clique_number_algorithm.h"

namespace cavcom {
  namespace graph {

    // Executes the Edwards/Elphick algorithm to determine a lower bound for the clique number of a graph.
    class CliqueEdwards : public CliqueNumberAlgorithm {
     public:
      // Creates a algorithm instance for the specified graph.
      explicit CliqueEdwards(const SimpleGraph &graph);

     protected:
      // Calls the base class method and then runs the algorithm.
      virtual bool run();

     private:
      VertexNumbers clique_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_CLIQUE_EDWARDS_H_
