// Creates and exercises simple graphs.

#include <libunittest/all.hpp>

#include <vector>

#include "graph.h"

using namespace cavcom::graph;

static constexpr VertexNumber ORDER = 4;

static void check_degrees(const Graph &g, Degree inmin, Degree inmax, Degree outmin, Degree outmax) {
  UNITTEST_ASSERT_EQUAL(g.minindeg(), inmin);
  UNITTEST_ASSERT_EQUAL(g.maxindeg(), inmax);
  UNITTEST_ASSERT_EQUAL(g.minoutdeg(), outmin);
  UNITTEST_ASSERT_EQUAL(g.maxoutdeg(), outmax);
  UNITTEST_ASSERT_EQUAL(g.mindeg(), g.minoutdeg());
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), g.maxoutdeg());
}

TEST(make_simple_empty) {
  Graph g(ORDER);

  // Check for a simple graph.
  UNITTEST_ASSERT_FALSE(g.directed());
  UNITTEST_ASSERT_FALSE(g.multiple());
  UNITTEST_ASSERT_FALSE(g.loops());

  // Check for proper order and size.
  UNITTEST_ASSERT_EQUAL(g.order(), ORDER);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);

  // Make sure that each vertex is unlabeled.
  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    const Vertex &v = g.vertex(iv);
    UNITTEST_ASSERT_EQUAL(v.id(), iv);
    UNITTEST_ASSERT_TRUE(v.label().empty());
    UNITTEST_ASSERT_EQUAL(v.color(), BLACK);
    UNITTEST_ASSERT_TRUE(v.contracted().empty());
  }

  // All degrees should be 0.
  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_EQUAL(g.indeg(iv), 0);
    UNITTEST_ASSERT_EQUAL(g.outdeg(iv), 0);
    UNITTEST_ASSERT_EQUAL(g.degree(iv), 0);
  }
  check_degrees(g, 0, 0, 0, 0);

  // Make sure that asking for an invalid vertex or edge throws an exception.
  UNITTEST_ASSERT_THROW(std::out_of_range, [&](){ g.vertex(ORDER); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&](){ g.edge(0); });
}

using EdgeValues = struct { VertexNumber from; VertexNumber to; const Label label; Color color; Weight weight; };
using EdgeData = std::vector<EdgeValues>;

static const EdgeData EDGEDATA = {{0, 1, "e1", 0, 1.0},
                                  {0, 2, Label(), 3, 0.0},
                                  {0, 3, "e3", 1, 0.5}};

TEST(add_simple_edges) {
  Graph g(ORDER);

  // By vertex number.
  EdgeNumber m = EDGEDATA.size();
  for (EdgeNumber ie = 0; ie < m; ++ie) {
    const EdgeValues &e = EDGEDATA[ie];
    UNITTEST_ASSERT_EQUAL(g.join(e.from, e.to, e.label, e.color, e.weight), 1);
  }

  // Check for proper order and size.
  UNITTEST_ASSERT_EQUAL(g.order(), ORDER);
  UNITTEST_ASSERT_EQUAL(g.size(), 3);

  // Check the edges.
  for (EdgeNumber ie = 0; ie < m; ++ie) {
    const EdgeValues &values = EDGEDATA[ie];
    const Edge &edge = g.edge(ie);
    UNITTEST_ASSERT_EQUAL(edge.from(), g.vertex(values.from).id());
    UNITTEST_ASSERT_EQUAL(edge.to(), g.vertex(values.to).id());
    UNITTEST_ASSERT_EQUAL(edge.label(), values.label);
    UNITTEST_ASSERT_EQUAL(edge.color(), values.color);
    UNITTEST_ASSERT_EQUAL(edge.weight(), values.weight);
  }

  // Check the degrees.
  UNITTEST_ASSERT_EQUAL(g.degree(0), 3);
  UNITTEST_ASSERT_EQUAL(g.degree(1), 1);
  UNITTEST_ASSERT_EQUAL(g.degree(2), 1);
  UNITTEST_ASSERT_EQUAL(g.degree(3), 1);

  check_degrees(g, 1, 3, 1, 3);
}
