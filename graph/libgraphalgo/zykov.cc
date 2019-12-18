#include "zykov.h"

namespace cavcom {
  namespace graph {

    Zykov::Zykov(const SimpleGraph &graph) : VertexColoringAlgorithm(graph) {}

    bool Zykov::run() {
      VertexColoringAlgorithm::run();
      current_.reset(new SimpleGraph(graph()));
      branch(graph());
      set_chromatic();
      return true;
    }

    void Zykov::branch(const SimpleGraph &state) {
      add_call();

      // Check for bounding.
      if (bound(state)) {
        done_call();
        return;
      }

      // Check for a complete graph (leaf state).
      if (state.complete()) {
        if (state.order() < current_->order()) current_.reset(new SimpleGraph(state));
        done_call();
        return;
      }

      // Find the first two nonadjacent vertices.  Note that because of the previous complete graph test, there
      // must be at least one such pair.
      VertexNumber n = state.order();
      VertexNumber last = n - 1;
      VertexNumber u = 0;
      VertexNumber v = 0;
      bool more = true;
      for (VertexNumber iv = 0; more && (iv < last); ++iv) {
        for (VertexNumber jv = iv + 1; more && (jv < n); ++jv) {
          if (!state.adjacent(iv, jv)) {
            u = iv;
            v = jv;
            more = false;
          }
        }
      }

      // Branch: contraction.
      SimpleGraph contract(state, u, v);
      branch(contract);

      // Branch: edge addition.
      SimpleGraph addition(state);
      addition.join(u, v);
      branch(addition);

      done_call();
    }

    void Zykov::set_chromatic() {
      const SimpleGraph &result = *current_;
      VertexNumber n = result.order();
      coloring_.resize(n);
      for (VertexNumber iv = 0; iv < n; ++iv) {
        const Vertex &v = result.vertex(iv);
        const Contracted &c = v.contracted();
        VertexNumber nc = c.size();
        VertexNumbers &color = coloring_[iv];
        if (nc > 0) {
          graph().ids_to_numbers(c, &color);
        } else {
          VertexIDs single = {v.id()};
          graph().ids_to_numbers(single, &color);
        }
      }
      number_ = n;
    }

  }  // namespace graph
}  // namespace cavcom
