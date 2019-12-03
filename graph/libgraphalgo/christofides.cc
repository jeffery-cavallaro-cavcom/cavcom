#include <algorithm>

#include "bron2.h"

#include "christofides.h"

namespace cavcom {
  namespace graph {

    class ChristofidesNode : public Bron2 {
     public:
      ChristofidesNode(Christofides *parent, const Christofides::Chromatic &chromatic, const SimpleGraph &subgraph)
        : Bron2(subgraph, MODE_ALL, false), parent_(parent), chromatic_(chromatic) {}

     private:
      Christofides *parent_;
      const Christofides::Chromatic &chromatic_;

      virtual bool found(const VertexNumbers &mis) {
        parent_->add_call();

        // The current MIS is in terms of vertex numbers relative to the current subgraph.  Convert it to vertex IDs.
        Christofides::IDs mis_ids;
        for_each(mis.cbegin(), mis.cend(), [&](VertexNumber iv){ mis_ids.insert(graph().vertex(iv).id()); });

        // Use a new color for the current MIS.
        parent_->next_->push_back(chromatic_);
        Christofides::Colorings::iterator next_chromatic = --parent_->next_->end();
        next_chromatic->coloring_.push_back(mis_ids);
        next_chromatic->vertices_.insert(mis_ids.cbegin(), mis_ids.cend());

        // Reject subsets.
        Christofides::Colorings::iterator ic = parent_->next_->begin();
        while (ic != next_chromatic) {
          Christofides::IDs::size_type shared = 0;
          for_each(next_chromatic->vertices_.cbegin(), next_chromatic->vertices_.cend(), [&](VertexID id){
              if (ic->vertices_.find(id) != ic->vertices_.cend()) ++shared;
            });
          if (shared == next_chromatic->vertices_.size()) {
            parent_->next_->erase(next_chromatic);
            parent_->done_call();
            return true;
          } else if (shared == ic->vertices_.size()) {
            Christofides::Colorings::iterator i = ic;
            ++ic;
            parent_->next_->erase(i);
          } else {
            ++ic;
          }
        }

        // If all the vertices have been used then the coloring is complete.
        bool more = true;
        if (next_chromatic->vertices_.size() >= parent_->graph().order()) {
          parent_->chromatic_ = next_chromatic->coloring_;
          more = false;
        }

        parent_->done_call();
        return more;
      }
    };

    Christofides::Christofides(const SimpleGraph &graph) : GraphAlgorithm(graph) {}

    bool Christofides::run() {
      current_.reset();
      next_.reset();
      chromatic_.clear();

      // Guard against a null graph.
      if (graph().order() == 0) return true;

      // A MIS in a graph is a clique in the complement.
      SimpleGraph complement(graph(), true);

      // Seed the algorithm with an empty coloring.
      current_.reset(new Colorings);
      current_->emplace_back(Coloring(), IDs());

      // Continue extending each current chromatic coloring until the first occasion when all vertices are used.
      bool more = true;
      while (more) {
        next_.reset(new Colorings);

        for (Colorings::const_iterator ic = current_->cbegin(); more && (ic != current_->cend()); ++ic) {
          // Construct the subgraph from which the subsequent MISs will be extracted to extend the current
          // k-chromatic coloring.
          VertexNumbers vremove;
          complement.ids_to_numbers(ic->vertices_, &vremove);
          SimpleGraph subgraph(complement, vremove, EdgeNumbers());

          // Process all MISs for the current subgraph.
          ChristofidesNode node(this, *ic, subgraph);
          more = node.execute();
        }

        current_ = std::move(next_);
      }

      // The chromatic coloring has been saved, so release the context.
      current_.reset();
      next_.reset();

      return true;
    }

  }  // namespace graph
}  // namespace cavcom
