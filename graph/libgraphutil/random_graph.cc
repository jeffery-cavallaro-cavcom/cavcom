#include <cmath>
#include <chrono>
#include <random>

#include "random_graph.h"

namespace cavcom {
  namespace graph {

    RandomGraph::RandomGraph(VertexNumber order, double eprob) : SimpleGraph(order), eprob_(eprob) {
      if (eprob_ < 0.0) eprob_ = 0.0;
      if (eprob_ > 1.0) eprob_ = 1.0;

      std::default_random_engine rng(make_seed());
      std::bernoulli_distribution dist(eprob_);

      VertexNumber last = order - 1;
      for (VertexNumber i = 0; i < last; ++i) {
        for (VertexNumber j = i + 1; j < order; ++j) {
          if (dist(rng)) join(i, j);
        }
      }
    }

    RandomGraph(VertexNumber order, double phigh, double plow, double pstep) {
      std::default_random_engine rng(make_seed());

      uint np = std::round((phigh - plow)/pstep);
    }

    uint RandomGraph::make_seed(void) {
      return std::chrono::system_clock::now().time_since_epoch().count();
    }

  }  // namespace graph
}  // namespace cavcom
