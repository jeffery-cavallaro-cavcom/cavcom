#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_MYCIELSKI_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_MYCIELSKI_H_

#include <memory>

#include "simple_graph.h"

namespace cavcom {
  namespace graph {
    // The Mycielski construction is used to obtain a triangle-free graph with arbitrarity high chromatic number.
    class Mycielski : public SimpleGraph {
     public:
      // Constructs a Mycielski graph with chromatic number k.  This is a recursive process as follows:
      //
      //  1.  Start with G equal to the trivial graph.
      //  2.  For each i from 1 to k, recursively construct a shadow graph: each vertex v in V(G) results in a
      //      vertex u such that N(u)=N(v).
      //  3.  Add a vertex w that is adjacent to all the added u.
      //
      Mycielski(Color k);

      // Calculates the order of a Mycielski construction for a given k:
      //
      //  n(k=0) = 0
      //  n(k=1) = 1
      //  n(k) = 3(2^(k-2))-1.
      //
      static VertexNumber n_of_k(Color k);
    };
  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_MYCIELSKI_H_
