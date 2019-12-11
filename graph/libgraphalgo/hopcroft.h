#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_HOPCROFT_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_HOPCROFT_H_

#include <vector>

#include "component_algorithm.h"

namespace cavcom {
  namespace graph {

    // Implements the Hopcroft Tarjan algorithm to identify the components in a graph.
    class Hopcroft : public ComponentAlgorithm {
     public:
      // Creates a new instance of a Hopcroft component algorithm for the specified graph.
      explicit Hopcroft(const SimpleGraph &graph, bool save = true);

     protected:
      // Calls the base class method and runs the algorithm.
      virtual bool run();

     private:
      std::vector<bool> used_;
      VertexNumber nunused_;

      // Recursive routine to find and add vertices to components.
      void add_to_component(VertexNumber iv, VertexNumbers *next);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_HOPCROFT_H_
