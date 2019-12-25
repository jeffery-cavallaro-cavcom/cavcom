#include <algorithm>
#include <vector>

#include "chromatic_wang.h"

#include "bron2.h"

namespace cavcom {
  namespace graph {

    ChromaticWang::ChromaticWang(const SimpleGraph &graph)
      : VertexColoringAlgorithm(graph), complement_(graph, true) {}

    bool ChromaticWang::run() {
      VertexColoringAlgorithm::run();

      // Guard against a null graph.
      if (graph().order() == 0) return true;

      // A MIS in a graph is a clique in the complement.
      SimpleGraph complement(graph(), true);

      // Seed the algorithm with the root state.
      next_states(VertexIDsList(), VertexIDs());

      return true;
    }

    void ChromaticWang::next_states(const VertexIDsList &coloring, VertexIDs vertices) {
      add_call();

      // Determine if this is the last chance for this subtree.
      bool last = (number_ > 0) && ((coloring.size() + 1) >= number_);

      // Construct the subgraph from which the subsequent MISs will be extracted.
      VertexNumbers vremove;
      complement_.ids_to_numbers(vertices, &vremove);
      SimpleGraph subgraph(complement_, vremove, EdgeNumbers());

      // Generate the MISs for the next states.
      Bron2 all_miss(subgraph);
      all_miss.execute();

      // Count the number of times that each vertex appears in a MIS.
      VertexNumber n = subgraph.order();
      std::vector<VertexNumbersList::size_type> in_miss(n);
      std::for_each(all_miss.cliques().cbegin(), all_miss.cliques().cend(),
                    [&in_miss](const VertexNumbers &mis){
                      std::for_each(mis.cbegin(), mis.cend(), [&in_miss](VertexNumber iv){ ++in_miss[iv]; });
                    });

      // Pick the vertex that appears in the fewest cliques.
      VertexNumber target = 0;
      VertexNumbersList::size_type nc = 0;
      for (VertexNumber iv = 0; iv < n; ++iv) {
        VertexNumbersList::size_type count = in_miss[iv];
        if (count < nc) {
          target = iv;
          nc = count;
        }
      }

      // Only select the MISs that contain the identified vertex.
      VertexNumbersList selected_miss;
      std::for_each(all_miss.cliques().cbegin(), all_miss.cliques().cend(),
                    [target, &selected_miss](const VertexNumbers &mis){
                      if (mis.find(target) != mis.cend()) selected_miss.push_back(mis);
                    });

      // Process each of the selected MISs.
      for (VertexNumbersList::const_iterator imis = selected_miss.cbegin(); imis != selected_miss.cend(); ++imis) {
        // The current MIS is in terms of vertex numbers relative to the current subgraph.  Convert it to vertex IDs.
        VertexIDs mis_ids;
        for_each(imis->cbegin(), imis->cend(),
                 [&mis_ids, &subgraph](VertexNumber iv){ mis_ids.insert(subgraph.vertex(iv).id()); });

        // Construct the next state from the current MIS.
        VertexIDsList next_coloring = coloring;
        next_coloring.push_back(mis_ids);

        VertexIDs next_vertices = vertices;
        next_vertices.insert(mis_ids.cbegin(), mis_ids.cend());

        // Check for done.
        if (next_vertices.size() >= graph().order()) {
          if ((number_ == 0) || (next_coloring.size() < number_)) {
            // This is the new smallest coloring thus far.
            chromatic(next_coloring);
          }
          continue;
        }

        // If the smallest coloring has not been exceeded then extend the current subtree.
        if (!last) next_states(next_coloring, next_vertices);
      }

      done_call();
    }

    void ChromaticWang::chromatic(const VertexIDsList &coloring) {
      coloring_.clear();
      for_each(coloring.cbegin(), coloring.cend(),
               [this](const VertexIDs &ids){
                 coloring_.resize(coloring_.size() + 1);
                 graph().ids_to_numbers(ids, &coloring_.back());
               });
      number_ = coloring_.size();
    }

  }  // namespace graph
}  // namespace cavcom
