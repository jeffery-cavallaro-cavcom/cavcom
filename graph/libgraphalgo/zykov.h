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

     private:
      using ColoringByIDs = std::vector<VertexIDs>;
      using GraphPtr = std::unique_ptr<SimpleGraph>;

      // The current smallest coloring.
      GraphPtr current_;

      // Calls the base class method and then runs the algorithm.
      virtual bool run();

      // The recursive branching method.  Calls the bounding method.  If the bounding method returns true then
      // the current branch is pruned and the method returns.  Otherwise, branching occurs.
      void branch(const SimpleGraph &state);

      // The bounding method for pruning subtrees.  Returns false to continue branching and true to prune the
      // current subtree.  The base class method always returns false (no bounding).  Derived methods can set their
      // own bounding criteria.
      virtual bool bound(const SimpleGraph &state) { return false; }

      // Transfers the final chromatic coloring to the base class attribute.
      void set_chromatic();
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_ZYKOV_H_
