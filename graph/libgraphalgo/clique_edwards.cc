#include <algorithm>

#include "clique_edwards.h"

namespace cavcom {
  namespace graph {

    CliqueEdwards::CliqueEdwards(const SimpleGraph &graph, bool smart)
      : CliqueNumberAlgorithm(graph), smart_(smart) {}

    bool CliqueEdwards::run() {
      // Initialize the base and derived contexts.
      CliqueNumberAlgorithm::run();
      clique_.clear();

      // Guard against the null graph.
      VertexNumber n = graph().order();
      if (n <= 0) return true;

      // Select the first vertex with maximum degree.
      Degree maxdeg = graph().maxdeg();
      VertexNumber selected = 0;
      for (VertexNumber iv = 0; iv < n; ++iv) {
        add_step();
        if (graph().degree(iv) >= maxdeg) {
          selected = iv;
          break;
        }
      }
      clique_.insert(selected);

      // Construct a clique from adjacent vertices.
      while (clique_.size() < n) {
        selected = 0;
        maxdeg = 0;

        bool found = false;
        for (VertexNumber iv = 0; iv < n; ++iv) {
          // Skip vertices that are not adjacant to everything in the current clique.  Note that this will skip
          // already selected vertices.
          if (std::any_of(clique_.cbegin(), clique_.cend(),
                          [&](VertexNumber jv){
                            add_step();
                            return !graph().adjacent(iv, jv);
                          })) {
            continue;
          }

          // The current vertex is adjacent to everything selected thus far.  If not smart then select the first
          // found vertex (at the lowest index).
          if (!smart_) {
            selected = iv;
            found = true;
            break;
          }

          // Look for the vertex with the highest degree.
          Degree d = graph().degree(iv);
          if (d > maxdeg) {
            selected = iv;
            maxdeg = d;
            found = true;
          }
        }

        // Stop if nothing found.  Otherwise, add the found vertex to the current clique.
        if (!found) break;
        clique_.insert(selected);
      }

      // Return the order of the found clique.
      number_ = clique_.size();
      return true;
    }

  }  // namespace graph
}  // namespace cavcom
