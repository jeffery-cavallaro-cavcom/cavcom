// Applies a coloring to a graph.

#include <libunittest/all.hpp>

#include "vertex_coloring_algorithm.h"

using namespace cavcom::graph;

// A dummy coloring algorithm that just sets the coloring.
class DummyAlgorithm : public VertexColoringAlgorithm {
 public:
  DummyAlgorithm(const SimpleGraph &graph, const Coloring &coloring) : VertexColoringAlgorithm(graph) {
    coloring_ = coloring;
  }

 private:
  virtual bool run(void) { return true; }
};

static const VertexValuesList VERTICES = {{"a", NOCOLOR, 0, 0},
                                          {"b", 1, 0, 0},
                                          {"c", 1, 0, 0},
                                          {"d", 1, 0, 0}};

static const DummyAlgorithm::Coloring COLORING = {{0}, {1, 3}, {2}};

TEST(color_graph) {
  SimpleGraph g(VERTICES, EdgeValuesList());
  DummyAlgorithm da(g, COLORING);
  da.apply(&g);
  UNITTEST_ASSERT_EQUAL(g.vertex(0).color(), 1);
  UNITTEST_ASSERT_EQUAL(g.vertex(1).color(), 2);
  UNITTEST_ASSERT_EQUAL(g.vertex(2).color(), 3);
  UNITTEST_ASSERT_EQUAL(g.vertex(3).color(), 2);
}
