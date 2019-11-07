#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_RANDOM_GRAPH_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_RANDOM_GRAPH_H_

#include "graph.h"

namespace cavcom {
  namespace graph {

    // Generate a random graph with a given edge probability.
    class RandomGraph : public Graph {
     public:
      // Creates a new random graph with the specified order and Bernoulli edge probability.  The probability is a
      // number between 0.0 and 1.0, inclusive.  Negative values are adjusted to 0 and values >1 are adjusted to 1.
      // The seed for the random number generator is obtained from the system time.
      RandomGraph(VertexNumber order, double eprob);

     private:
      double eprob_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_RANDOM_GRAPH_H_
