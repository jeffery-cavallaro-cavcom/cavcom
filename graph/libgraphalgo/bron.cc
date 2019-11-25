#include "bron.h"

namespace cavcom {
  namespace graph {

    const int Bron::MODE_ALL;
    const int Bron::MODE_MAX;
    const int Bron::MODE_MAX_ONLY;

    Bron::Bron(const SimpleGraph &graph, int mode) : GraphAlgorithm(graph), mode_(mode), total_(0), number_(0) {}

    bool Bron::run() {
      // Reset the context.
      current_.clear();
      cliques_.clear();
      total_ = 0;
      number_ = 0;

      // Guard against a null graph.
      VertexNumber n = graph().order();
      if (n <= 0) return true;

      // At first, all vertices are candidates and nothing has been previously used.
      VertexNumberList candidates, used;
      candidates.reserve(n);
      VertexNumber iv = n;
      while (iv > 0) candidates.push_back(--iv);

      // Run the algorithm.
      return extend(&candidates, &used);
    }

    bool Bron::add_clique(void) {
      VertexNumber n = current_.size();
      if (mode() > 0) {
        cliques_.push_back(current_);
      } else {
        if (cliques_.empty() || (n > cliques_[0].size())) {
          cliques_.clear();
          cliques_.push_back(current_);
        }
      }
      ++total_;
      if (n > number_) number_ = n;
      return found(current_);
    }

  }  // namespace graph
}  // namespace cavcom
