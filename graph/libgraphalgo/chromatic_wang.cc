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
      next_states(ColoringByIDs(), VertexIDs());

      return true;
    }

    void ChromaticWang::next_states(const ColoringByIDs &coloring, VertexIDs vertices) {
      add_call();

      // Determine if this is the last chance for this subtree.
      bool last = (number_ > 0) && ((coloring.size() + 1) >= number_);

      // Construct the subgraph from which the subsequent MISs will be extracted.
      VertexNumbers vremove;
      complement_.ids_to_numbers(vertices, &vremove);
      SimpleGraph subgraph(complement_, vremove, EdgeNumbers());

      // Generate the MISs for the next states.
      Bron2 all_mis(subgraph);
      all_mis.execute();

      // Count the number of times that each vertex appears in a MIS.
      VertexNumber n = subgraph.order();
      std::vector<Bron::Cliques::size_type> in_mis(n);
      std::for_each(all_mis.cliques().cbegin(), all_mis.cliques().cend(),
                    [&](const VertexNumbers &mis){
                      std::for_each(mis.cbegin(), mis.cend(), [&](VertexNumber iv){ ++in_mis[iv]; });
                    });

      // Pick the vertex that appears in the fewest cliques.
      VertexNumber v = 0;
      Bron::Cliques::size_type nc = 0;
      for (VertexNumber iv = 0; iv < n; ++iv) {
        Bron::Cliques::size_type count = in_mis[iv];
        if (count < nc) {
          v = iv;
          nc = count;
        }
      }

      // Only select the MISs that contain the identified vertex.
      Bron::Cliques current_mis;
      std::for_each(all_mis.cliques().cbegin(), all_mis.cliques().cend(),
                    [&](const VertexNumbers &next){
                      if (next.find(v) != next.cend()) current_mis.push_back(next);
                    });

      // Process each of the selected MISs.
      for (Bron::Cliques::const_iterator imis = current_mis.cbegin(); imis != current_mis.cend(); ++imis) {
        // The current MIS is in terms of vertex numbers relative to the current subgraph.  Convert it to vertex IDs.
        VertexIDs mis_ids;
        for_each(imis->cbegin(), imis->cend(), [&](VertexNumber iv){ mis_ids.insert(subgraph.vertex(iv).id()); });

        // Construct the next state from the current MIS.
        ColoringByIDs next_coloring = coloring;
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

    void ChromaticWang::chromatic(const ColoringByIDs &coloring) {
      coloring_.clear();
      for_each(coloring.cbegin(), coloring.cend(),
               [&](const VertexIDs &ids){
                 coloring_.resize(coloring_.size() + 1);
                 graph().ids_to_numbers(ids, &coloring_.back());
               });
      number_ = coloring_.size();
    }

  }  // namespace graph
}  // namespace cavcom
