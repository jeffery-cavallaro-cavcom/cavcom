#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_

#include <list>
#include <memory>
#include <set>
#include <vector>

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // The Christofides algorithm for finding the chromatic number of a graph.  This algorithm is based on the
    // fact that a k-chromatic subgraph S of a graph G can be constructed from a (k-1)-chromatic subgraph and a
    // maximal independent set of vertices from G-V(S).
    class Christofides : public GraphAlgorithm {
     public:
      using IDs = std::set<VertexID>;
      using Coloring = std::vector<IDs>;

      // Create a new Christofides algorithm instance for the specified graph.
      Christofides(const SimpleGraph &graph);

      // Returns the first found chromatic coloring.
      const Coloring &chromatic(void) const { return chromatic_; }

      // Returns the chromatic number of the graph.
      Coloring::size_type number(void) const { return chromatic_.size(); }

     private:
      // A k-chromatic coloring that is to be extended to a (k+1)-chromatic coloring.  Note that all vertices are
      // identified by vertex ID, since vertex position will change as subgraphs are constructed.  A flat copy of
      // the vertices in the coloring is maintained for easier subset determination.
      struct Chromatic {
        Chromatic(const Coloring &coloring, const IDs &vertices) : coloring_(coloring), vertices_(vertices) {}

        Coloring coloring_;
        IDs vertices_;
      };
      using Colorings = std::list<Chromatic>;
      using ColoringsPtr = std::unique_ptr<Colorings>;

      ColoringsPtr current_;
      ColoringsPtr next_;
      Coloring chromatic_;

      virtual bool run();

      friend class ChristofidesNode;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_CHRISTOFIDES_H_
