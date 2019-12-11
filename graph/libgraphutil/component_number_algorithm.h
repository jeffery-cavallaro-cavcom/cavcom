#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_COMPONENT_NUMBER_ALGORITHM_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_COMPONENT_NUMBER_ALGORITHM_H_

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // Any algorithm that finds or estimates the number of components in a graph.
    class ComponentNumberAlgorithm : public GraphAlgorithm {
     public:
      // Creates a new number of components algorithm for the specified graph.
      explicit ComponentNumberAlgorithm(const SimpleGraph &graph);

      // Returns the found or estimated component number.
      uint number() const { return number_; }

     protected:
      uint number_;

      // Calls the base class method and resets the number of components.
      virtual bool run();
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_COMPONENT_NUMBER_ALGORITHM_H_
