#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_BRON2_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_BRON2_H_

#include "bron.h"

namespace cavcom {
  namespace graph {

    // Version 2 of the Bron-Kerbocsh-Schell algorithm for finding all of the maximal cliques in a graph.  This
    // version tries to be smarter when selected the next candidate to make sure that the bounding condition happens
    // as soon as possible.
    class Bron2 : public Bron {
     public:
      Bron2(const SimpleGraph &graph, int mode = MODE_ALL, bool save = true);

     private:
      virtual bool extend(VertexNumberList *pcandidates, VertexNumberList *pused);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_BRON2_H_
