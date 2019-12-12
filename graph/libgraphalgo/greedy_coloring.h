#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_GREEDY_COLORING_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_GREEDY_COLORING_H_

#include <vector>

#include "vertex_coloring_algorithm.h"

namespace cavcom {
  namespace graph {

    // Any greedy (sequential) vertex coloring algorithm.  Derived classes determine the vertex order.
    class GreedyColoring : public VertexColoringAlgorithm {
     public:
      // Create a new greedy coloring algorithm instance for the specified graph.
      GreedyColoring(const SimpleGraph &graph, bool interchange = true);

      // Returns true if color interchange is enabled.
      bool interchange(void) const { return interchange_; }

     protected:
      // The sorted vertex list.
      using VertexNumberList = std::vector<VertexNumber>;
      VertexNumberList sorted_;

      // Vertex color assignments.
      using ColorAssignments = std::vector<Color>;
      ColorAssignments colors_;

      // Sorts the vertices into the desired coloring order.  The default is highest degree first.
      virtual void sort(void);

     private:
      // Enables color interchange.
      bool interchange_;

      // Calls the base class method and then runs the algorithm.
      virtual bool run();

      // Attempts to perform a color interchange.
      Color attempt_interchange(VertexNumber iv);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_GREEDY_COLORING_H_
