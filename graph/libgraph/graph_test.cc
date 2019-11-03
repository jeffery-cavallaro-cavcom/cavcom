// Creates and exercises simple graphs.

#include <libunittest/all.hpp>

#include <vector>

#include "graph.h"

using namespace cavcom::graph;

static constexpr VertexNumber ORDER = 4;

static void check_vertex(const Vertex &vertex, VertexID id, const VertexValues &values) {
  UNITTEST_ASSERT_EQUAL(vertex.id(), id);
  UNITTEST_ASSERT_EQUAL(vertex.label(), values.label);
  UNITTEST_ASSERT_EQUAL(vertex.color(), values.color);
  UNITTEST_ASSERT_EQUAL(vertex.xpos(), values.xpos);
  UNITTEST_ASSERT_EQUAL(vertex.ypos(), values.ypos);
}

static void check_edge(const Edge &edge, VertexID from, VertexID to, const EdgeValues &values) {
  UNITTEST_ASSERT_EQUAL(edge.from(), from);
  UNITTEST_ASSERT_EQUAL(edge.to(), to);
  UNITTEST_ASSERT_EQUAL(edge.label(), values.label);
  UNITTEST_ASSERT_EQUAL(edge.color(), values.color);
  UNITTEST_ASSERT_EQUAL(edge.weight(), values.weight);
}

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
    UNITTEST_ASSERT_EQUAL(v.xpos(), 0.0);
    UNITTEST_ASSERT_EQUAL(v.ypos(), 0.0);
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

static const VertexValuesList VERTICES = {{"v1", BLACK, 0, 0},
                                          {"v2", 1, 0, 1},
                                          {"v3", 2, 1, 1},
                                          {"v4", 3, 1, 0}};

static const EdgeValuesList EDGES = {{0, 1, "e1", 0, 1.0},
                                     {0, 2, Label(), 3, 0.0},
                                     {0, 3, "e3", 1, 0.5}};

TEST(make_simple_graph) {
  Graph g(VERTICES, EDGES);

  // Check for proper order and size.
  UNITTEST_ASSERT_EQUAL(g.order(), ORDER);
  UNITTEST_ASSERT_EQUAL(g.size(), 3);

  // Check the vertices.
  for (VertexNumber iv = 0; iv < VERTICES.size(); ++iv) {
    const VertexValues &v = VERTICES[iv];
    check_vertex(g.vertex(iv), iv, v);
    VertexNumber found = 0;
    UNITTEST_ASSERT_TRUE(g.find_vertex(iv, &found));
    UNITTEST_ASSERT_EQUAL(found, iv);
    found = 0;
    UNITTEST_ASSERT_TRUE(g.find_vertex(v.label, &found));
    UNITTEST_ASSERT_EQUAL(found, iv);
  }

  // Check the edges.
  for (EdgeNumber ie = 0; ie < EDGES.size(); ++ie) {
    const EdgeValues &values = EDGES[ie];
    check_edge(g.edge(ie), values.from, values.to, values);
  }

  // Check adjacencies.
  UNITTEST_ASSERT_FALSE(g.adjacent(0, 0));
  UNITTEST_ASSERT_TRUE(g.adjacent(0, 1));
  UNITTEST_ASSERT_TRUE(g.adjacent(0, 2));
  UNITTEST_ASSERT_TRUE(g.adjacent(0, 3));

  UNITTEST_ASSERT_TRUE(g.adjacent(1, 0));
  UNITTEST_ASSERT_FALSE(g.adjacent(1, 1));
  UNITTEST_ASSERT_FALSE(g.adjacent(1, 2));
  UNITTEST_ASSERT_FALSE(g.adjacent(1, 3));

  UNITTEST_ASSERT_TRUE(g.adjacent(2, 0));
  UNITTEST_ASSERT_FALSE(g.adjacent(2, 1));
  UNITTEST_ASSERT_FALSE(g.adjacent(2, 2));
  UNITTEST_ASSERT_FALSE(g.adjacent(2, 3));

  UNITTEST_ASSERT_TRUE(g.adjacent(3, 0));
  UNITTEST_ASSERT_FALSE(g.adjacent(3, 1));
  UNITTEST_ASSERT_FALSE(g.adjacent(3, 2));
  UNITTEST_ASSERT_FALSE(g.adjacent(3, 3));

  // Check the degrees.
  UNITTEST_ASSERT_EQUAL(g.degree(0), 3);
  UNITTEST_ASSERT_EQUAL(g.degree(1), 1);
  UNITTEST_ASSERT_EQUAL(g.degree(2), 1);
  UNITTEST_ASSERT_EQUAL(g.degree(3), 1);

  check_degrees(g, 1, 3, 1, 3);
}

static const VertexValuesList KV = {{"v1"}, {"v2"}, {"v3"}, {"v4"}, {"v5"}};

static const EdgeValuesList KE = {{0, 1, "e0"}, {0, 2, "e1"}, {0, 3, "e2"}, {0, 4, "e3"},
                                  {1, 2, "e4"}, {1, 3, "e5"}, {1, 4, "e6"},
                                  {2, 3, "e7"}, {2, 4, "e8"},
                                  {3, 4, "e9"}};

TEST(make_simple_subgraph) {
  // Start with a complete graph.
  Graph kg(KV, KE);
  UNITTEST_ASSERT_EQUAL(kg.order(), KV.size());
  UNITTEST_ASSERT_EQUAL(kg.size(), KE.size());
  check_degrees(kg, 4, 4, 4, 4);

  // Remove two of the vertices.
  const VertexNumbers out = {1, 3};
  Graph sg(kg, out);
  UNITTEST_ASSERT_EQUAL(sg.order(), KV.size() - 2);
  UNITTEST_ASSERT_EQUAL(sg.size(), KE.size() - 7);
  check_degrees(sg, 2, 2, 2, 2);

  // Check the remaining.
  for (VertexNumber iv = 0; iv < kg.order(); ++iv) {
    const Vertex v = kg.vertex(iv);
    VertexNumber found;
    if (out.find(v.id()) == out.cend()) {
      UNITTEST_ASSERT_TRUE(sg.find_vertex(v.id(), &found));
      const Vertex &sv = sg.vertex(found);
      UNITTEST_ASSERT_EQUAL(sv.id(), v.id());
      UNITTEST_ASSERT_EQUAL(sv.label(), v.label());
      UNITTEST_ASSERT_EQUAL(sv.color(), BLACK);
      UNITTEST_ASSERT_EQUAL(sv.xpos(), 0.0);
      UNITTEST_ASSERT_EQUAL(sv.ypos(), 0.0);
    } else {
      UNITTEST_ASSERT_FALSE(sg.find_vertex(v.id(), &found));
    }
  }

  // The remaining vertices should all be adjacent.
  UNITTEST_ASSERT_FALSE(sg.adjacent(0, 0));
  UNITTEST_ASSERT_TRUE(sg.adjacent(0, 1));
  UNITTEST_ASSERT_TRUE(sg.adjacent(0, 2));

  UNITTEST_ASSERT_TRUE(sg.adjacent(1, 0));
  UNITTEST_ASSERT_FALSE(sg.adjacent(1, 1));
  UNITTEST_ASSERT_TRUE(sg.adjacent(1, 2));

  UNITTEST_ASSERT_TRUE(sg.adjacent(2, 0));
  UNITTEST_ASSERT_TRUE(sg.adjacent(2, 1));
  UNITTEST_ASSERT_FALSE(sg.adjacent(2, 2));

  // All degrees should be 2.
  for (VertexNumber iv = 0; iv < sg.order(); ++iv) {
    UNITTEST_ASSERT_EQUAL(sg.degree(iv), 2);
  }
}
