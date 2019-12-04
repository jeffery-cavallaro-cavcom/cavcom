#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_

#include <list>
#include <memory>
#include <set>
#include <vector>

#include "vertex_coloring_algorithm.h"

namespace cavcom {
  namespace graph {

    // The Christofides algorithm for finding the chromatic number of a graph.  This algorithm is based on the
    // fact that a k-chromatic subgraph S of a graph G can be constructed from a (k-1)-chromatic subgraph and a
    // maximal independent set of vertices from G-V(S).
    class Christofides : public VertexColoringAlgorithm {
     public:
      using ColoringByIDs = std::vector<VertexIDs>;

      // Create a new Christofides algorithm instance for the specified graph.
      Christofides(const SimpleGraph &graph);

     private:
      // A k-chromatic coloring that is to be extended to a (k+1)-chromatic coloring.  Note that all vertices are
      // identified by vertex ID, since vertex position will change as subgraphs are constructed.  A flat copy of
      // the vertices in the coloring is maintained for easier subset determination.
      struct Chromatic {
        Chromatic(const ColoringByIDs &coloring, const VertexIDs &vertices)
          : coloring_(coloring), vertices_(vertices) {}

        ColoringByIDs coloring_;
        VertexIDs vertices_;
      };
      using Colorings = std::list<Chromatic>;
      using ColoringsPtr = std::unique_ptr<Colorings>;

      ColoringsPtr current_;
      ColoringsPtr next_;

      // Calls the base class method and then runs the algorithm.
      virtual bool run();

      // Converts the found coloring from IDs to numbers for return.
      void ids_to_numbers(const ColoringByIDs &coloring);

      friend class ChristofidesNode;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_
