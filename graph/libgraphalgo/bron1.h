#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_BRON1_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_BRON1_H_

#include "bron.h"

namespace cavcom {
  namespace graph {

    // Version 1 of the Bron-Kerbocsh-Schell algorithm for finding all of the maximal cliques in a graph.
    class Bron1 : public Bron {
     public:
      Bron1(const SimpleGraph &graph, int mode = MODE_ALL);

     private:
      virtual void extend(VertexNumberList *pcandidates, VertexNumberList *pused);
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_BRON1_H_
