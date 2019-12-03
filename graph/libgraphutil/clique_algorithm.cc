#include "clique_algorithm.h"

namespace cavcom {
  namespace graph {

    const int CliqueAlgorithm::MODE_ALL;
    const int CliqueAlgorithm::MODE_MAX_ONLY;
    const int CliqueAlgorithm::MODE_FIRST_MAX;

    CliqueAlgorithm::CliqueAlgorithm(const SimpleGraph &graph, int mode, bool save)
      : CliqueNumberAlgorithm(graph), mode_(mode), save_(save), total_(0) {}

    bool CliqueAlgorithm::add_clique(const VertexNumbers &next) {
      if (!found(next)) return false;
      VertexNumber n = next.size();
      bool bigger = (n > number_);
      bool keep = false;
      if (save_) {
        if (mode_ > 0) {
          keep = true;
        } else if (mode_ == 0) {
          if (bigger) cliques_.clear();
          keep = (n >= number_);
        } else {
          if (bigger) {
            cliques_.clear();
            keep = true;
          }
        }
      }
      if (keep) cliques_.push_back(next);
      if (bigger) number_ = n;
      ++total_;
      return true;
    }

    bool CliqueAlgorithm::run() {
      CliqueNumberAlgorithm::run();
      cliques_.clear();
      total_ = 0;
      return true;
    }

  }  // namespace graph
}  // namespace cavcom
