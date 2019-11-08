#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_

#include <memory>

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // A modified version of aZykov algorithm to determine the chromatic number of a graph.  The algorithm is
    // composed of an outer method that loops on increasing values of k and a called method that determines if the
    // current state of the graph is k-colorable.  The first such k found is the chromatic number.
    class QuickZykov : public GraphAlgorithm {
     public:
      QuickZykov(const Graph &g);

      // Returns the current k value.  If the algorithm is complete then this is the chromatic number.
      uint k() const { return k_; }

      // Returns the final complete graph that represents a chromatic coloring of the original G, or null if the
      // algorithm has not yet completed.
      const Graph &chromatic() const { return *chromatic_; }

     private:
      using GraphPtr = std::unique_ptr<Graph>;

      uint k_;
      GraphPtr chromatic_;

      virtual bool run();

      // The steps of the outer loop are as follows:
      //
      //  1. Check for a null graph (n=0) condition.  If so, stop with k = 0.
      //
      //  2. Check for an empty graph (m=0) condition.  If so, stop with k = 1.
      //
      //  3. Initialize k to 2.
      //
      //  4. Call the method to determine if the current state of G is k-colorable, for the current value of k.  If
      //     so, then return with the current value of k.  If not, then the method will return a subgraph replacement
      //     for G to be used for the remainder of the algorithm (see the called method for details).
      //
      //  5. Increment k and go to step 4.
      //
      // Each of these steps is counted.

      void outer_loop(GraphPtr *ppg);

      bool is_k_colorable(GraphPtr *ppsg);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
