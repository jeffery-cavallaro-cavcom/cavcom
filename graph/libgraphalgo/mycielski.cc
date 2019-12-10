#include "mycielski.h"

namespace cavcom {
  namespace graph {

    Mycielski::Mycielski(Color k) : SimpleGraph(n_of_k(k)) {
      // Nothing to do for the empty or trivial graphs.
      if (k < 2) return;

      // There is at least a path.
      join(0, 1);

      // Construct the shadow graph with an extra vertex.
      VertexNumber prev_n = 2;
      for (Color ic = 3; ic <= k; ++ic) {
        VertexNumber n = 2*prev_n;
        for (VertexNumber iv = 0, is = iv + prev_n; iv < prev_n; ++iv, ++is) {
          for (VertexNumber jv = 0; jv < prev_n; ++jv) {
            if (adjacent(iv, jv)) join(is, jv);
          }
          join(n, is);
        }
        prev_n = n + 1;
      }
    }

    VertexNumber Mycielski::n_of_k(Color k) {
      return (k <= 2) ? k : 3*(1 << (k - 2)) - 1;
    }

  }  // namespace graph
}  // namespace cavcom
