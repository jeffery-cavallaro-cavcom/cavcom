// Creates and exercises simple graphs.

#include <vector>

#include <libunittest/all.hpp>

#include "errors.h"
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

TEST(make_null_graph) {
  Graph g(0);
  UNITTEST_ASSERT_TRUE(g.null());
}

TEST(make_simple_empty) {
  Graph g(ORDER);
  UNITTEST_ASSERT_FALSE(g.null());
  UNITTEST_ASSERT_TRUE(g.empty());

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

  // Remove two of the vertices.  Also remove two edges: one that would have been removed due to the removed
  // vertices and one that would have stayed.
  VertexNumbers vout = {1, 3};
  EdgeNumbers eout = {4, 3};
  Graph sg(kg, vout, eout);
  UNITTEST_ASSERT_EQUAL(sg.order(), 3);
  UNITTEST_ASSERT_EQUAL(sg.size(), 2);
  check_degrees(sg, 1, 2, 1, 2);

  // Check the remaining.
  for (VertexNumber iv = 0; iv < kg.order(); ++iv) {
    const Vertex v = kg.vertex(iv);
    VertexNumber found;
    if (vout.find(v.id()) == vout.cend()) {
      UNITTEST_ASSERT_TRUE(sg.find_vertex(v.id(), &found));
      const Vertex &sv = sg.vertex(found);
      UNITTEST_ASSERT_EQUAL(sv.id(), v.id());
      UNITTEST_ASSERT_EQUAL(sv.label(), v.label());
      UNITTEST_ASSERT_EQUAL(sv.color(), BLACK);
      UNITTEST_ASSERT_EQUAL(sv.xpos(), 0.0);
      UNITTEST_ASSERT_EQUAL(sv.ypos(), 0.0);
      UNITTEST_ASSERT_TRUE(sv.contracted().empty());
    } else {
      UNITTEST_ASSERT_FALSE(sg.find_vertex(v.id(), &found));
    }
  }

  // Check the remaining adjacenies.
  UNITTEST_ASSERT_FALSE(sg.adjacent(0, 0));
  UNITTEST_ASSERT_TRUE(sg.adjacent(0, 1));
  UNITTEST_ASSERT_FALSE(sg.adjacent(0, 2));

  UNITTEST_ASSERT_TRUE(sg.adjacent(1, 0));
  UNITTEST_ASSERT_FALSE(sg.adjacent(1, 1));
  UNITTEST_ASSERT_TRUE(sg.adjacent(1, 2));

  UNITTEST_ASSERT_FALSE(sg.adjacent(2, 0));
  UNITTEST_ASSERT_TRUE(sg.adjacent(2, 1));
  UNITTEST_ASSERT_FALSE(sg.adjacent(2, 2));

  // Check the degrees.
  UNITTEST_ASSERT_EQUAL(sg.degree(0), 1);
  UNITTEST_ASSERT_EQUAL(sg.degree(1), 2);
  UNITTEST_ASSERT_EQUAL(sg.degree(2), 1);
}

TEST(remove_all_vertices) {
  // Start with a complete graph.
  Graph kg(KV, KE);
  UNITTEST_ASSERT_EQUAL(kg.order(), KV.size());
  UNITTEST_ASSERT_EQUAL(kg.size(), KE.size());
  UNITTEST_ASSERT_FALSE(kg.null());
  check_degrees(kg, 4, 4, 4, 4);

  // Remove all of the vertices.
  const VertexNumbers out = {0, 4, 2, 3, 1};
  Graph sg(kg, out);
  UNITTEST_ASSERT_EQUAL(sg.order(), 0);
  UNITTEST_ASSERT_EQUAL(sg.size(), 0);
  UNITTEST_ASSERT_TRUE(sg.null());
  check_degrees(sg, 0, 0, 0, 0);
}

TEST(contract_vertices) {
  // Start with a complete graph.
  Graph kg(KV, KE);
  UNITTEST_ASSERT_EQUAL(kg.order(), KV.size());
  UNITTEST_ASSERT_EQUAL(kg.size(), KE.size());
  UNITTEST_ASSERT_FALSE(kg.empty());

  UNITTEST_ASSERT_EQUAL(kg.vertex(0).id(), 0);
  UNITTEST_ASSERT_EQUAL(kg.vertex(1).id(), 1);
  UNITTEST_ASSERT_EQUAL(kg.vertex(2).id(), 2);
  UNITTEST_ASSERT_EQUAL(kg.vertex(3).id(), 3);
  UNITTEST_ASSERT_EQUAL(kg.vertex(4).id(), 4);

  UNITTEST_ASSERT_EQUAL(kg.edge(0).from(), 0);
  UNITTEST_ASSERT_EQUAL(kg.edge(0).to(), 1);
  UNITTEST_ASSERT_EQUAL(kg.edge(1).from(), 0);
  UNITTEST_ASSERT_EQUAL(kg.edge(1).to(), 2);
  UNITTEST_ASSERT_EQUAL(kg.edge(2).from(), 0);
  UNITTEST_ASSERT_EQUAL(kg.edge(2).to(), 3);
  UNITTEST_ASSERT_EQUAL(kg.edge(3).from(), 0);
  UNITTEST_ASSERT_EQUAL(kg.edge(3).to(), 4);
  UNITTEST_ASSERT_EQUAL(kg.edge(4).from(), 1);
  UNITTEST_ASSERT_EQUAL(kg.edge(4).to(), 2);
  UNITTEST_ASSERT_EQUAL(kg.edge(5).from(), 1);
  UNITTEST_ASSERT_EQUAL(kg.edge(5).to(), 3);
  UNITTEST_ASSERT_EQUAL(kg.edge(6).from(), 1);
  UNITTEST_ASSERT_EQUAL(kg.edge(6).to(), 4);
  UNITTEST_ASSERT_EQUAL(kg.edge(7).from(), 2);
  UNITTEST_ASSERT_EQUAL(kg.edge(7).to(), 3);
  UNITTEST_ASSERT_EQUAL(kg.edge(8).from(), 2);
  UNITTEST_ASSERT_EQUAL(kg.edge(8).to(), 4);
  UNITTEST_ASSERT_EQUAL(kg.edge(9).from(), 3);
  UNITTEST_ASSERT_EQUAL(kg.edge(9).to(), 4);

  VertexNumber d = kg.order() - 1;
  for (VertexNumber iv = 0; iv < kg.order(); ++iv) {
    UNITTEST_ASSERT_EQUAL(kg.degree(iv), d);
  }
  check_degrees(kg, d, d, d, d);

  // Contract two of the vertices.
  Graph cg1(kg, 0, 1);
  UNITTEST_ASSERT_EQUAL(cg1.order(), KV.size() - 1);
  UNITTEST_ASSERT_EQUAL(cg1.size(), KE.size() - 4);
  UNITTEST_ASSERT_FALSE(cg1.empty());

  UNITTEST_ASSERT_EQUAL(cg1.vertex(0).id(), 2);
  UNITTEST_ASSERT_EQUAL(cg1.vertex(1).id(), 3);
  UNITTEST_ASSERT_EQUAL(cg1.vertex(2).id(), 4);
  UNITTEST_ASSERT_EQUAL(cg1.vertex(3).id(), 5);

  UNITTEST_ASSERT_EQUAL(cg1.edge(0).from(), 5);
  UNITTEST_ASSERT_EQUAL(cg1.edge(0).to(), 2);
  UNITTEST_ASSERT_EQUAL(cg1.edge(1).from(), 5);
  UNITTEST_ASSERT_EQUAL(cg1.edge(1).to(), 3);
  UNITTEST_ASSERT_EQUAL(cg1.edge(2).from(), 5);
  UNITTEST_ASSERT_EQUAL(cg1.edge(2).to(), 4);
  UNITTEST_ASSERT_EQUAL(cg1.edge(3).from(), 2);
  UNITTEST_ASSERT_EQUAL(cg1.edge(3).to(), 3);
  UNITTEST_ASSERT_EQUAL(cg1.edge(4).from(), 2);
  UNITTEST_ASSERT_EQUAL(cg1.edge(4).to(), 4);
  UNITTEST_ASSERT_EQUAL(cg1.edge(5).from(), 3);
  UNITTEST_ASSERT_EQUAL(cg1.edge(5).to(), 4);

  d = cg1.order() - 1;
  for (VertexNumber iv = 0; iv < cg1.order(); ++iv) {
    UNITTEST_ASSERT_EQUAL(cg1.degree(iv), d);
  }
  check_degrees(cg1, d, d, d, d);

  Contracted c1 = {0, 1};
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg1.vertex(d).contracted(), c1);

  // Contract two more.
  Graph cg2(cg1, 3, 1);
  UNITTEST_ASSERT_EQUAL(cg2.order(), cg1.order() - 1);
  UNITTEST_ASSERT_EQUAL(cg2.size(), cg1.size() - 3);
  UNITTEST_ASSERT_FALSE(cg2.empty());

  UNITTEST_ASSERT_EQUAL(cg2.vertex(0).id(), 2);
  UNITTEST_ASSERT_EQUAL(cg2.vertex(1).id(), 4);
  UNITTEST_ASSERT_EQUAL(cg2.vertex(2).id(), 6);

  UNITTEST_ASSERT_EQUAL(cg2.edge(0).from(), 6);
  UNITTEST_ASSERT_EQUAL(cg2.edge(0).to(), 2);
  UNITTEST_ASSERT_EQUAL(cg2.edge(1).from(), 6);
  UNITTEST_ASSERT_EQUAL(cg2.edge(1).to(), 4);
  UNITTEST_ASSERT_EQUAL(cg2.edge(2).from(), 2);
  UNITTEST_ASSERT_EQUAL(cg2.edge(2).to(), 4);

  d = cg2.order() - 1;
  for (VertexNumber iv = 0; iv < cg2.order(); ++iv) {
    UNITTEST_ASSERT_EQUAL(cg2.degree(iv), d);
  }
  check_degrees(cg2, d, d, d, d);

  c1.push_back(3);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg2.vertex(d).contracted(), c1);

  // And once more.
  Graph cg3(cg2, 1, 0);
  UNITTEST_ASSERT_EQUAL(cg3.order(), cg2.order() - 1);
  UNITTEST_ASSERT_EQUAL(cg3.size(), cg2.size() - 2);
  UNITTEST_ASSERT_FALSE(cg3.empty());

  UNITTEST_ASSERT_EQUAL(cg3.vertex(0).id(), 6);
  UNITTEST_ASSERT_EQUAL(cg3.vertex(1).id(), 7);

  UNITTEST_ASSERT_EQUAL(cg3.edge(0).from(), 6);
  UNITTEST_ASSERT_EQUAL(cg3.edge(0).to(), 7);

  d = cg3.order() - 1;
  for (VertexNumber iv = 0; iv < cg3.order(); ++iv) {
    UNITTEST_ASSERT_EQUAL(cg3.degree(iv), d);
  }
  check_degrees(cg3, d, d, d, d);

  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg3.vertex(0).contracted(), c1);
  Contracted c2 = {4, 2};
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg3.vertex(1).contracted(), c2);

  // And finally down to one vertex.
  Graph cg4(cg3, 0, 1);
  UNITTEST_ASSERT_EQUAL(cg4.order(), 1);
  UNITTEST_ASSERT_EQUAL(cg4.size(), 0);
  UNITTEST_ASSERT_TRUE(cg4.empty());

  UNITTEST_ASSERT_EQUAL(cg4.vertex(0).id(), 8);
  UNITTEST_ASSERT_EQUAL(cg4.degree(0), 0);
  check_degrees(cg4, 0, 0, 0, 0);

  c1.insert(c1.end(), c2.cbegin(), c2.cend());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg4.vertex(0).contracted(), c1);

  // Trying to contract a single vertex throws an exception.
  UNITTEST_ASSERT_THROW(SameVertexContractError, [&](){ Graph cg5(cg4, 0, 0); });
}
