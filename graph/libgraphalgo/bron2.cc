#include "bron2.h"

namespace cavcom {
  namespace graph {

    Bron2::Bron2(const SimpleGraph &graph, int mode) : Bron(graph, mode) {}

    void Bron2::extend(VertexNumberList *pcandidates, VertexNumberList *pused) {
      add_call();

      const SimpleGraph &g = graph();
      VertexNumberList &candidates = *pcandidates;
      VertexNumberList &used = *pused;

      // If only trying to determine the clique number then abandon branches that don't have enough candidates to
      // make a clique that exceeds the current maximum.
      if (mode() < 0) {
        if (current_.size() + candidates.size() <= number()) {
          done_call();
          return;
        }
      }

      // The vertex with the least amount of adjacencies to the candidate vertices.
      VertexNumber target = 0;

      // The position in the used or candidate list of the target vertex.
      VertexNumberList::size_type target_pos = 0;

      // The number of candidates that are not adjacent to the target vertex.
      VertexNumberList::size_type count = 0;

      // False: the target is in the used list; True: the target is in the candidate list.
      bool is_a_candidate = false;

      // Look for a used vertex that has the least number of adjacencies with the candidate vertices.
      VertexNumberList::size_type nc = candidates.size();
      VertexNumberList::size_type nu = used.size();
      for (VertexNumberList::size_type iu = 0; iu < nu; ++iu) {
        VertexNumber next_used = used[iu];
        VertexNumberList::size_type next_count = 0;
        for (VertexNumberList::size_type ic = 0; ic < nc; ++ic) {
          if (!g.adjacent(next_used, candidates[ic])) {
            ++next_count;
          }
        }
        if ((iu == 0) || (next_count < count)) {
          target = next_used;
          target_pos = iu;
          count = next_count;
        }
      }

      // If there exists a used vertex that is adjacent to every candidate vertex then the remaining candidates will
      // never be able to construct a maximal clique.
      if ((nu > 0) && (count == 0)) {
        done_call();
        return;
      }

      // See if there are any candidates with even less adjacencies.  Be sure to count the fact that a vertex is not
      // adjacent to iteself.
      for (VertexNumberList::size_type ic = 0; ic < nc; ++ic) {
        VertexNumber next_candidate = candidates[ic];
        VertexNumberList::size_type next_count = 0;
        for (VertexNumberList::size_type jc = 0; jc < nc; ++jc) {
          if (!g.adjacent(next_candidate, candidates[jc])) {
            ++next_count;
          }
        }
        if (((nu == 0) && (ic == 0)) || (next_count < count)) {
          target = next_candidate;
          target_pos = ic;
          count = next_count;
          is_a_candidate = true;
        }
      }

      // Keep going as long as the target used vertex is not adjacent to at least one candidate.
      while (count > 0) {
        // Select the next candidate and swap it into the next select position at the end of the candidate list.
        VertexNumber &last = candidates.back();
        VertexNumberList::size_type last_pos = candidates.size() - 1;
        if (is_a_candidate) {
          // The target vertex is still in the candidate list.  Make sure that it is the next one selected.
          if (target_pos != last_pos) {
            candidates[target_pos] = last;
            last = target;
            target_pos = last_pos;
          }
        } else {
          // Find a candidate vertex that is not adjacent to the used target.  There should be at least one.  This
          // will decrease the target count when the selected vertex gets moved to the used list.
          for (VertexNumberList::size_type ic = 0; ic < nc; ++ic) {
            VertexNumber next = candidates[ic];
            if (!g.adjacent(target, next)) {
              if (ic != last_pos) {
                candidates[ic] = last;
                last = next;
              }
              break;
            }
          }
        }

        // Add the next candidate to the current clique.
        VertexNumber selected = candidates.back();
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

        // All done with the current selected vertex.  Move it to the used list.
        current_.pop_back();
        candidates.pop_back();
        --nc;
        used.push_back(selected);
        ++nu;

        // If the target was previously a candidate then mark it as the current target in the used list.
        if (is_a_candidate) {
          target = used.back();
          target_pos = used.size() - 1;
          is_a_candidate = false;
        }

        // The target now has one less nonadjacency.
        --count;
      }

      // All the candidates for this level have been tried.  Accept the current clique if it is maximal.
      if (used.empty()) add_clique();
      done_call();
      return;
    }

  }  // namespace graph
}  // namespace cavcom
