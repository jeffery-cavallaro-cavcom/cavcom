#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_CHROMATIC_NUMBER_ALGORITHM_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_CHROMATIC_NUMBER_ALGORITHM_H_

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // Any algorithm that finds or estimates the chromatic number for a graph.
    class ChromaticNumberAlgorithm : public GraphAlgorithm {
     public:
      // Creates a new chromatic number algorithm for the specified graph.  The graph must remain valid during the
      // lifetime of the algorithm.
      explicit ChromaticNumberAlgorithm(const SimpleGraph &graph);

      // Returns the found or estimated chromatic number.
      Color number() const { return number_; }

     protected:
      Color number_;

      // Calls the base class method and resets the chromatic number.
      virtual bool run();
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_CHROMATIC_NUMBER_ALGORITHM_H_
