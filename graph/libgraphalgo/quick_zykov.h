#ifndef CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
#define CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_

#include "graph_algorithm.h"

namespace cavcom {
  namespace graph {

    // A modified version of Zykov algorithm that loops on values of k and only resorts to recursive calls if a
    // sequence of bounding checks fails.
    class QuickZykov : public GraphAlgorithm {
     public:
      QuickZykov(const Graph &graph);

     private:
      uint k_;

      virtual bool run();
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHALGO_QUICK_ZYKOV_H_
