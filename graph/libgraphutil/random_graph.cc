#include <chrono>
#include <random>

#include "random_graph.h"

namespace cavcom {
  namespace graph {

    RandomGraph::RandomGraph(VertexNumber order, double eprob) : SimpleGraph(order), eprob_(eprob) {
      if (eprob_ < 0.0) eprob_ = 0.0;
      if (eprob_ > 1.0) eprob_ = 1.0;

      uint seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::default_random_engine rng(seed);
      std::bernoulli_distribution dist(eprob_);

      VertexNumber last = order - 1;
      for (VertexNumber i = 0; i < last; ++i) {
        for (VertexNumber j = i + 1; j < order; ++j) {
          if (dist(rng)) join(i, j);
        }
      }
    }

  }  // namespace graph
}  // namespace cavcom
