#include "bron1.h"

namespace cavcom {
  namespace graph {

    Bron1::Bron1(const SimpleGraph &graph) : GraphAlgorithm(graph) {}

    bool Bron1::run() {
      // Reset the context.
      cliques_.clear();
      current_.clear();

      // Guard against a null graph.
      VertexNumber n = graph().order();
      if (n <= 0) return true;

      // At first, all vertices are candidates and nothing has been previously used.
      VertexNumberList candidates, used;
      candidates.reserve(n);
      VertexNumber iv = n;
      while (iv > 0) candidates.push_back(--iv);

      // Run the algorithm.
      extend(&candidates, &used);

      // Always successful.
      return true;
    }

    void Bron1::extend(VertexNumberList *pcandidates, VertexNumberList *pused) {
      add_call();

      const SimpleGraph &g = graph();
      VertexNumberList &candidates = *pcandidates;
      VertexNumberList &used = *pused;

      // Try all of the candidates.
      while (!candidates.empty()) {
        // Make sure that there is not something in the used list that is adjacent to everything in the candidates
        // list.  Otherwise, any cliques constructed from this point will never be maximal.
        VertexNumberList::size_type nc = candidates.size();
        VertexNumberList::size_type nu = used.size();
        for (VertexNumberList::size_type iu = 0; iu < nu; ++iu) {
          VertexNumber u = used[iu];
          bool all_adj = true;
          for (VertexNumberList::size_type ic = 0; (ic < nc) & all_adj; ++ic) {
            all_adj = g.adjacent(u, candidates[ic]);
          }
          if (all_adj) {
            done_call();
            return;
          }
        }

        // Add the next candidate to the current clique.
        VertexNumber selected = candidates.back();
        candidates.pop_back();
        --nc;
        current_.push_back(selected);

        // Only keep additional candidates that are adjacent to the current selected candidate.
        VertexNumberList next_candidates;
        for (VertexNumberList::size_type ic = 0; ic < nc; ++ic) {
          VertexNumber c = candidates[ic];
          if (g.adjacent(selected, c)) next_candidates.push_back(c);
        }

        // Only keep used vertices that are still adjacent to the current clique.
        VertexNumberList next_used;
        for (VertexNumberList::size_type iu = 0; iu < nu; ++iu) {
          VertexNumber u = used[iu];
          if (g.adjacent(selected, u)) next_used.push_back(u);
        }

        // Find all maximal cliques that extend the current clique.
        extend(&next_candidates, &next_used);

        // All done with the current selected vertex, so mark it used.
        current_.pop_back();
        used.push_back(selected);
      }

      // All the candidates for this level have been tried.  Mark the current clique if it is maximal.
      if (used.empty()) cliques_.push_back(current_);
      done_call();
      return;
    }

  }  // namespace graph
}  // namespace cavcom
