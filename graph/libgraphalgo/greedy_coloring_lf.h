#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_GREEDY_COLORING_LF_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_GREEDY_COLORING_LF_H_

#include "vertex_coloring_algorithm.h"

namespace cavcom {
  namespace graph {

    // A greedy (sequential) vertex coloring algorithm that colors largest degree vertices first.
    class GreedyColoringLF : public VertexColoringAlgorithm {
     public:
      // Create a new greedy coloring algorithm instance for the specified graph.
      GreedyColoringLF(const SimpleGraph &graph);

     private:
      // Calls the base class method and then runs the algorithm.
      virtual bool run();
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_GREEDY_COLORING_LF_H_
