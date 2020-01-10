#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_RANDOM_GRAPH_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_RANDOM_GRAPH_H_

#include "simple_graph.h"

namespace cavcom {
  namespace graph {

    // Generate a random graph with a given edge probability.
    class RandomGraph : public SimpleGraph {
     public:
      // Creates a new random graph with the specified order and Bernoulli edge probability.  The probability is a
      // number between 0.0 and 1.0, inclusive.  Negative values are adjusted to 0 and values >1 are adjusted to 1.
      // The seed for the random number generator is obtained from the system time.
      RandomGraph(VertexNumber order, double eprob);

      // Creates a new random graph with a non-uniform edge probability for each vertex.  First, the vertices are
      // randomly shuffled.  A total of nep = (phigh - plow)/pstep probability buckets are created.  Each bucket is
      // assigned n/nep vertices (from the shuffled list), with the extra n % nep vertices assigned to the lowest
      // edge priority.  Edges are then assigned in shuffled order.  If a vertex already exceeds its assigned
      // edge density then no additional edges are assigned.
      RandomGraph(VertexNumber order, double phigh, double plow, double pstep);

     private:
      double eprob_;

      // Generates a date/time-based seed for random number generation.
      uint make_seed(void);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_RANDOM_GRAPH_H_
