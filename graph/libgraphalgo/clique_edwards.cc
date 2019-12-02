#include <algorithm>

#include "clique_edwards.h"

namespace cavcom {
  namespace graph {

    CliqueEdwards::CliqueEdwards(const SimpleGraph &graph) : CliqueAlgorithm(graph) {}

    bool CliqueEdwards::run() {
      // Initialize the base context.
      if (!CliqueAlgorithm::run()) return false;

      // Guard against the null graph.
      VertexNumber n = graph().order();
      if (n <= 0) return true;

      // Start with nothing selected.
      std::vector<bool> selected(n);

      // Select the first vertex with maximum degree.
      add_step();
      VertexNumber iv = 0;
      Degree maxdeg = graph().maxdeg();
      while ((iv < n) && (graph().degree(iv) < maxdeg)) ++iv;
      clique_.insert(iv);
      selected[iv] = true;

      // Add vertices, lowest index first, that are pairwise adjacent to the current clique.
      iv = 0;
      while (iv < n) {
        add_step();
        if (!selected[iv]) {
          if (std::all_of(clique_.cbegin(), clique_.cend(),
                          [&](VertexNumber jv){
                            add_step();
                            return graph().adjacent(iv, jv);
                          })) {
            clique_.insert(iv);
            selected[iv] = true;
            iv = 0;
            continue;
          }
        }
        ++iv;
      }

      // Return the found clique.
      return true;
    }

  }  // namespace graph
}  // namespace cavcom
