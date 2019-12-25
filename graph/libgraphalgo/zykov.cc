#include "zykov.h"

#include "clique_edwards.h"
#include "greedy_coloring.h"

namespace cavcom {
  namespace graph {

    Zykov::Zykov(const SimpleGraph &graph) : VertexColoringAlgorithm(graph) {}

    bool Zykov::run() {
      // Initialize the base and derived contexts.
      VertexColoringAlgorithm::run();
      current_.reset(new SimpleGraph(graph()));

      // Establish the initial upper bound.
      set_upper_bound();

      // Branch and bound.
      branch(graph());

      // If something other than the default was found then return it.
      set_chromatic();

      return true;
    }

    void Zykov::set_upper_bound() {
      GreedyColoring greedy(graph());
      greedy.execute();
      current_.reset(new SimpleGraph(graph(), greedy.coloring()));
      number_ = greedy.number();
    }

    bool Zykov::bound(const SimpleGraph &state) {
      // The upper bound is no worse than this state.
      if (state.order() < number_) number_ = state.order();

      // Check this state's lower bound.
      CliqueEdwards ce(state);
      ce.execute();

      return ce.number() >= number_;
    }

    void Zykov::branch(const SimpleGraph &state) {
      add_call();

      // Check for a complete graph (leaf state).
      if (state.complete()) {
        if (state.order() < current_->order()) current_.reset(new SimpleGraph(state));
        done_call();
        return;
      }

      // Check for bounding.
      if (bound(state)) {
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
        const VertexIDs &c = v.contracted();
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
