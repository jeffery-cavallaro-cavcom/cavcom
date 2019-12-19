#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_ZYKOV_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_ZYKOV_H_

#include <memory>
#include <vector>

#include "vertex_coloring_algorithm.h"

namespace cavcom {
  namespace graph {

    // A Zykov algorithm for finding the chromatic number of a graph.  Zykov algorithms are branch-and-bound
    // algorithms based the fact that: X(G)=min{X(G.uv), X(G+uv)}
    class Zykov : public VertexColoringAlgorithm {
     public:
      // Create a new Zykov algorithm instance for the specified graph.
      Zykov(const SimpleGraph &graph);

     protected:
      // Calls the base class method and then runs the algorithm.
      virtual bool run();

      // The bounding method for pruning subtrees.  Returns false to continue branching and true to prune the
      // current subtree.  The default method maintains a current upper bound and uses the Edwards Elphick
      // algorithm to determine a lower bound.  If the lower bound exceeds the current upper bound then the
      // subtree is pruned.
      virtual bool bound(const SimpleGraph &state);

     private:
      using ColoringByIDs = std::vector<VertexIDs>;
      using GraphPtr = std::unique_ptr<SimpleGraph>;

      // The current smallest coloring.
      GraphPtr current_;

      // Establishes the upper bound.
      void set_upper_bound();

      // The recursive branching method.  Calls the bounding method.  If the bounding method returns true then
      // the current branch is pruned and the method returns.  Otherwise, branching occurs.
      void branch(const SimpleGraph &state);

      // Transfers the final chromatic coloring to the base class attribute.
      void set_chromatic();
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_ZYKOV_H_
