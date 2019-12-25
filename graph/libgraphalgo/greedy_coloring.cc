#include "greedy_coloring.h"

#include <algorithm>
#include <set>
#include <vector>

#include "hopcroft.h"

namespace cavcom {
  namespace graph {

    GreedyColoring::GreedyColoring(const SimpleGraph &graph, bool interchange)
      : VertexColoringAlgorithm(graph),
        sorted_(graph.order()), colors_(graph.order(), NOCOLOR), interchange_(interchange) {}

    bool GreedyColoring::run() {
      // Initialize the base and derived context.
      VertexColoringAlgorithm::run();

      // Guard against an empty graph.
      VertexNumber n = graph().order();
      if (n <= 0) return true;

      // Sort the vertices into the desired coloring order.
      for (VertexNumber iv = 0; iv < n; ++iv) sorted_[iv] = iv;
      sort();

      // Color the vertices, largest first.
      for (VertexNumber iv = 0; iv < n; ++iv) {
        VertexNumber next = sorted_[iv];

        // Determine which colors are used by all adjacent vertices that have already been colored.
        std::vector<bool> used(number_ + 1);
        for (VertexNumber jv = 0; jv < iv; ++jv) {
          add_step();
          VertexNumber prev = sorted_[jv];
          if (graph().adjacent(next, prev)) used[colors_[prev]] = true;
        }

        // Use the first unused color.
        Color ic = 0;
        for (ic = 0; used[ic]; ++ic) { add_step(); }
        if ((ic >= number_) && interchange_ && (iv >= 4)) ic = attempt_interchange(iv);
        colors_[next] = ic;
        if (ic >= number_) number_ = ic + 1;
      }

      // Extract the coloring.
      coloring_.resize(number_);
      for (VertexNumber iv = 0; iv < n; ++iv) {
        coloring_[colors_[iv]].insert(iv);
      }

      return true;
    }

    void GreedyColoring::sort(void) {
      std::sort(sorted_.begin(), sorted_.end(),
                [this](VertexNumber iv, VertexNumber jv){ return graph().degree(iv) > graph().degree(jv); });
    }

    Color GreedyColoring::attempt_interchange(VertexNumber iv) {
      // Determine which vertices are adjacent to the target vertex and what colors are used.
      VertexNumber next = sorted_[iv];
      VertexNumbers adj;
      std::vector<Degree> used(number_);
      for (VertexNumber jv = 0; jv < iv; ++jv) {
        add_step();
        VertexNumber prev = sorted_[jv];
        if (graph().adjacent(next, prev)) {
          adj.insert(prev);
          ++used[colors_[prev]];
        }
      }

      // Select all adjacent colors that appear just once.
      std::set<Color> once;
      for (Color ic = 0; ic < number_; ++ic) {
        add_step();
        if (used[ic] == 1) once.insert(ic);
      }

      // Make a list of all vertices colored with the identified colors and make a sub list that contains only
      // those vertices that are adjacent.
      VertexNumbers keep;
      VertexNumbers keep_adj;
      for (VertexNumber jv = 0; jv < iv; ++jv) {
        add_step();
        VertexNumber prev = sorted_[jv];
        if (once.find(colors_[prev]) != once.cend()) {
          keep.insert(prev);
          if (adj.find(prev) != adj.cend()) keep_adj.insert(prev);
        }
      }

      // Construct an induced subgraph from the found vertices and break it up into components.
      add_step();
      SimpleGraph subgraph(graph(), keep);
      Hopcroft hop(subgraph);
      hop.execute();
      const VertexNumbersList &components = hop.components();

      // Convert the kept adjacent vertex numbers to be relative to the subgraph.
      VertexNumbers sub_keep_adj;
      std::for_each(keep_adj.cbegin(), keep_adj.cend(),
                    [this, &subgraph, &sub_keep_adj](VertexNumber ig){
                      VertexNumber is = 0;
                      subgraph.find_vertex(graph().vertex(ig).id(), &is);
                      sub_keep_adj.insert(is);
                    });

      // Find a subgraph component that has at least one present and one missing kept adjacent vertex.
      auto nc = hop.number();
      decltype(hop.number()) ic = 0;
      VertexNumber present = 0;
      VertexNumber not_present = 0;

      for (ic = 0; ic < nc; ++ic) {
        present = 0;
        not_present = 0;
        Degree in_comp = 0;
        Degree not_in_comp = 0;

        const VertexNumbers &sub_comp = components[ic];
        std::for_each(sub_keep_adj.cbegin(), sub_keep_adj.cend(),
                      [this, &sub_comp, &in_comp, &not_in_comp, &present, &not_present](VertexNumber is){
                        add_step();
                        if (sub_comp.find(is) == sub_comp.cend()) {
                          if (not_in_comp <= 0) not_present = is;
                          ++not_in_comp;
                        } else {
                          if (in_comp <= 0) present = is;
                          ++in_comp;
                        }
                      });
        if ((in_comp > 0) && (not_in_comp > 0)) break;
      }

      // If no suitable component was found then just use the next color.
      if (ic >= nc) return number_;

      // A component was found with at least one present and one not present adjacent vertex.  Determine how these
      // vertices are colored in the original graph.
      VertexNumber vp = 0;
      VertexNumber vnp = 0;
      graph().find_vertex(subgraph.vertex(present).id(), &vp);
      graph().find_vertex(subgraph.vertex(not_present).id(), &vnp);
      Color c1 = colors_[vp];
      Color c2 = colors_[vnp];

      // Swap the two colors in the target component.
      const VertexNumbers &sub_comp = components[ic];
      std::for_each(sub_comp.cbegin(), sub_comp.cend(),
                    [this, &subgraph, c1, c2](VertexNumber is){
                      add_step();
                      VertexNumber ig = 0;
                      graph().find_vertex(subgraph.vertex(is).id(), &ig);
                      Color c = colors_[ig];
                      if (c == c1) {
                        colors_[ig] = c2;
                      } else if (c == c2) {
                        colors_[ig] = c1;
                      }
                    });

      // It is now OK to use c1 for the current vertex.
      return c1;
    }

  }  // namespace graph
}  // namespace cavcom
