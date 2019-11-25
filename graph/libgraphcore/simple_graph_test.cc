// Creates and exercises simple graphs.

#include <vector>

#include <libunittest/all.hpp>

#include "errors.h"
#include "simple_graph.h"

using namespace cavcom::graph;

TEST(make_null_graph) {
  SimpleGraph g(0);

  UNITTEST_ASSERT_FALSE(g.directed());
  UNITTEST_ASSERT_FALSE(g.multiple());
  UNITTEST_ASSERT_FALSE(g.loops());

  UNITTEST_ASSERT_EQUAL(g.order(), 0);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);
  UNITTEST_ASSERT_TRUE(g.null());
  UNITTEST_ASSERT_TRUE(g.empty());
  UNITTEST_ASSERT_TRUE(g.complete());

  UNITTEST_ASSERT_EQUAL(g.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), 0);
}

TEST(make_trivial_graph) {
  SimpleGraph g(1);

  UNITTEST_ASSERT_FALSE(g.directed());
  UNITTEST_ASSERT_FALSE(g.multiple());
  UNITTEST_ASSERT_FALSE(g.loops());

  UNITTEST_ASSERT_EQUAL(g.order(), 1);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);
  UNITTEST_ASSERT_FALSE(g.null());
  UNITTEST_ASSERT_TRUE(g.empty());
  UNITTEST_ASSERT_TRUE(g.complete());

  UNITTEST_ASSERT_EQUAL(g.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), 0);
}

static const VertexValuesList VERTICES = {{"v1", NOCOLOR, 0, 0},
                                          {"v2", 1, 0, 1},
                                          {"v3", 2, 1, 1},
                                          {"v4", 3, 1, 0}};

static const EdgeValuesList EDGES = {{0, 1, "e1", 0, 1.0},
                                     {0, 2, Label(), 3, 0.0},
                                     {0, 3, "e3", 1, 0.5}};

static constexpr VertexNumber ORDER = 4;

TEST(make_empty) {
  SimpleGraph g(ORDER);

  UNITTEST_ASSERT_FALSE(g.directed());
  UNITTEST_ASSERT_FALSE(g.multiple());
  UNITTEST_ASSERT_FALSE(g.loops());

  UNITTEST_ASSERT_EQUAL(g.order(), ORDER);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);
  UNITTEST_ASSERT_FALSE(g.null());
  UNITTEST_ASSERT_TRUE(g.empty());
  UNITTEST_ASSERT_FALSE(g.complete());

  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    const Vertex &v = g.vertex(iv);
    UNITTEST_ASSERT_EQUAL(v.id(), iv);
    UNITTEST_ASSERT_TRUE(v.label().empty());
    UNITTEST_ASSERT_EQUAL(v.color(), NOCOLOR);
    UNITTEST_ASSERT_TRUE(v.contracted().empty());
    UNITTEST_ASSERT_EQUAL(v.xpos(), 0.0);
    UNITTEST_ASSERT_EQUAL(v.ypos(), 0.0);
  }

  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_EQUAL(g.degree(iv), 0);
  }
  UNITTEST_ASSERT_EQUAL(g.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), 0);

  UNITTEST_ASSERT_THROW(std::out_of_range, [&](){ g.vertex(ORDER); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&](){ g.edge(0); });
}

TEST(make_graph) {
  SimpleGraph g(VERTICES, EDGES);

  UNITTEST_ASSERT_FALSE(g.directed());
  UNITTEST_ASSERT_FALSE(g.multiple());
  UNITTEST_ASSERT_FALSE(g.loops());

  UNITTEST_ASSERT_EQUAL(g.order(), ORDER);
  UNITTEST_ASSERT_EQUAL(g.size(), 3);
  UNITTEST_ASSERT_FALSE(g.null());
  UNITTEST_ASSERT_FALSE(g.empty());
  UNITTEST_ASSERT_FALSE(g.complete());

  for (VertexNumber iv = 0; iv < VERTICES.size(); ++iv) {
    const VertexValues &values = VERTICES[iv];
    const Vertex &vertex = g.vertex(iv);
    UNITTEST_ASSERT_EQUAL(vertex.id(), iv);
    UNITTEST_ASSERT_EQUAL(vertex.label(), values.label);
    UNITTEST_ASSERT_EQUAL(vertex.color(), values.color);
    UNITTEST_ASSERT_EQUAL(vertex.xpos(), values.xpos);
    UNITTEST_ASSERT_EQUAL(vertex.ypos(), values.ypos);

    VertexNumber found = 0;
    UNITTEST_ASSERT_TRUE(g.find_vertex(iv, &found));
    UNITTEST_ASSERT_EQUAL(found, iv);
    found = 0;
    UNITTEST_ASSERT_TRUE(g.find_vertex(values.label, &found));
    UNITTEST_ASSERT_EQUAL(found, iv);
  }

  for (EdgeNumber ie = 0; ie < EDGES.size(); ++ie) {
    const EdgeValues &values = EDGES[ie];
    const Edge &edge = g.edge(ie);
    UNITTEST_ASSERT_EQUAL(edge.from(), values.from);
    UNITTEST_ASSERT_EQUAL(edge.to(), values.to);
    UNITTEST_ASSERT_EQUAL(edge.label(), values.label);
    UNITTEST_ASSERT_EQUAL(edge.color(), values.color);
    UNITTEST_ASSERT_EQUAL(edge.weight(), values.weight);
  }

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

  UNITTEST_ASSERT_EQUAL(g.degree(0), 3);
  UNITTEST_ASSERT_EQUAL(g.degree(1), 1);
  UNITTEST_ASSERT_EQUAL(g.degree(2), 1);
  UNITTEST_ASSERT_EQUAL(g.degree(3), 1);

  UNITTEST_ASSERT_EQUAL(g.mindeg(), 1);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), 3);
}

TEST(make_copy) {
  SimpleGraph g(VERTICES, EDGES);
  SimpleGraph cg(g);

  UNITTEST_ASSERT_EQUAL(g.order(), cg.order());
  UNITTEST_ASSERT_EQUAL(g.size(), cg.size());

  UNITTEST_ASSERT_FALSE(cg.directed());
  UNITTEST_ASSERT_FALSE(cg.multiple());
  UNITTEST_ASSERT_FALSE(cg.loops());

  for (VertexNumber iv = 0; iv < g.order(); ++iv) {
    const Vertex &v = g.vertex(iv);
    const Vertex &cv = cg.vertex(iv);
    UNITTEST_ASSERT_EQUAL(v.id(), cv.id());
    UNITTEST_ASSERT_EQUAL(v.label(), cv.label());
    UNITTEST_ASSERT_EQUAL(v.color(), cv.color());
    UNITTEST_ASSERT_EQUAL(v.xpos(), cv.xpos());
    UNITTEST_ASSERT_EQUAL(v.ypos(), cv.ypos());
  }

  for (VertexNumber ie = 0; ie < g.size(); ++ie) {
    const Edge &e = g.edge(ie);
    const Edge &ce = cg.edge(ie);
    UNITTEST_ASSERT_EQUAL(e.from(), ce.from());
    UNITTEST_ASSERT_EQUAL(e.to(), ce.to());
    UNITTEST_ASSERT_EQUAL(e.label(), ce.label());
    UNITTEST_ASSERT_EQUAL(e.color(), ce.color());
    UNITTEST_ASSERT_EQUAL(e.weight(), ce.weight());
  }

  for (VertexNumber iv = 0; iv < g.order(); ++iv) {
    for (VertexNumber jv = 0; jv < g.order(); ++jv) {
      if (g.adjacent(iv, jv)) {
        UNITTEST_ASSERT_TRUE(cg.adjacent(iv, jv));
      } else {
        UNITTEST_ASSERT_FALSE(cg.adjacent(iv, jv));
      }
    }
  }

  UNITTEST_ASSERT_EQUAL(g.mindeg(), cg.mindeg());
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), cg.maxdeg());

  for (VertexNumber iv = 0; iv < g.order(); ++iv) {
    UNITTEST_ASSERT_EQUAL(g.degree(iv), cg.degree(iv));
  }
}

TEST(make_complete) {
  SimpleGraph g(VERTICES, EDGES);
  UNITTEST_ASSERT_FALSE(g.complete());
  g.make_complete();

  UNITTEST_ASSERT_EQUAL(g.order(), ORDER);
  UNITTEST_ASSERT_EQUAL(g.size(), ORDER*(ORDER - 1)/2);
  UNITTEST_ASSERT_FALSE(g.null());
  UNITTEST_ASSERT_FALSE(g.empty());
  UNITTEST_ASSERT_TRUE(g.complete());

  for (VertexNumber iv = 0; iv < ORDER - 1; ++iv) {
    for (VertexNumber jv = iv + 1; jv < ORDER; ++jv) {
      UNITTEST_ASSERT_TRUE(g.adjacent(iv, jv));
    }
  }

  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_EQUAL(g.degree(iv), ORDER - 1);
  }
  UNITTEST_ASSERT_EQUAL(g.mindeg(), ORDER - 1);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), ORDER - 1);
}

static const VertexValuesList KV = {{"v1"}, {"v2"}, {"v3"}, {"v4"}, {"v5"}};

static const EdgeValuesList KE = {{0, 1, "e0"}, {0, 2, "e1"}, {0, 3, "e2"}, {0, 4, "e3"},
                                  {1, 2, "e4"}, {1, 3, "e5"}, {1, 4, "e6"},
                                  {2, 3, "e7"}, {2, 4, "e8"},
                                  {3, 4, "e9"}};

TEST(make_subgraph_keep) {
  // Start with a complete graph.
  SimpleGraph kg(KV, KE);
  UNITTEST_ASSERT_EQUAL(kg.order(), KV.size());
  UNITTEST_ASSERT_EQUAL(kg.size(), KE.size());
  UNITTEST_ASSERT_EQUAL(kg.mindeg(), 4);
  UNITTEST_ASSERT_EQUAL(kg.maxdeg(), 4);

  // Keep only three of the vertices.
  VertexNumbers vkeep = {1, 3, 4};
  SimpleGraph sg(kg, vkeep);
  UNITTEST_ASSERT_EQUAL(sg.order(), 3);
  UNITTEST_ASSERT_EQUAL(sg.size(), 3);
  UNITTEST_ASSERT_EQUAL(sg.mindeg(), 2);
  UNITTEST_ASSERT_EQUAL(sg.maxdeg(), 2);

  // Check the remaining.
  for (VertexNumber iv = 0; iv < kg.order(); ++iv) {
    const Vertex v = kg.vertex(iv);
    VertexNumber found;
    if (vkeep.find(v.id()) == vkeep.cend()) {
      UNITTEST_ASSERT_FALSE(sg.find_vertex(v.id(), &found));
    } else {
      UNITTEST_ASSERT_TRUE(sg.find_vertex(v.id(), &found));
      const Vertex &sv = sg.vertex(found);
      UNITTEST_ASSERT_EQUAL(sv.id(), v.id());
      UNITTEST_ASSERT_EQUAL(sv.label(), v.label());
      UNITTEST_ASSERT_EQUAL(sv.color(), NOCOLOR);
      UNITTEST_ASSERT_EQUAL(sv.xpos(), 0.0);
      UNITTEST_ASSERT_EQUAL(sv.ypos(), 0.0);
      UNITTEST_ASSERT_TRUE(sv.contracted().empty());
    }
  }

  // Check the remaining adjacencies.
  UNITTEST_ASSERT_FALSE(sg.adjacent(0, 0));
  UNITTEST_ASSERT_TRUE(sg.adjacent(0, 1));
  UNITTEST_ASSERT_TRUE(sg.adjacent(0, 2));

  UNITTEST_ASSERT_TRUE(sg.adjacent(1, 0));
  UNITTEST_ASSERT_FALSE(sg.adjacent(1, 1));
  UNITTEST_ASSERT_TRUE(sg.adjacent(1, 2));

  UNITTEST_ASSERT_TRUE(sg.adjacent(2, 0));
  UNITTEST_ASSERT_TRUE(sg.adjacent(2, 1));
  UNITTEST_ASSERT_FALSE(sg.adjacent(2, 2));

  // Check the degrees.
  UNITTEST_ASSERT_EQUAL(sg.degree(0), 2);
  UNITTEST_ASSERT_EQUAL(sg.degree(1), 2);
  UNITTEST_ASSERT_EQUAL(sg.degree(2), 2);
}

TEST(make_subgraph_remove) {
  // Start with a complete graph.
  SimpleGraph kg(KV, KE);
  UNITTEST_ASSERT_EQUAL(kg.order(), KV.size());
  UNITTEST_ASSERT_EQUAL(kg.size(), KE.size());
  UNITTEST_ASSERT_EQUAL(kg.mindeg(), 4);
  UNITTEST_ASSERT_EQUAL(kg.maxdeg(), 4);

  // Remove two of the vertices.  Also remove two edges: one that would have been removed due to the removed
  // vertices and one that would have stayed.
  VertexNumbers vout = {1, 3};
  EdgeNumbers eout = {4, 3};
  SimpleGraph sg(kg, vout, eout);
  UNITTEST_ASSERT_EQUAL(sg.order(), 3);
  UNITTEST_ASSERT_EQUAL(sg.size(), 2);
  UNITTEST_ASSERT_EQUAL(sg.mindeg(), 1);
  UNITTEST_ASSERT_EQUAL(sg.maxdeg(), 2);

  // Check the remaining.
  for (VertexNumber iv = 0; iv < kg.order(); ++iv) {
    const Vertex v = kg.vertex(iv);
    VertexNumber found;
    if (vout.find(v.id()) == vout.cend()) {
      UNITTEST_ASSERT_TRUE(sg.find_vertex(v.id(), &found));
      const Vertex &sv = sg.vertex(found);
      UNITTEST_ASSERT_EQUAL(sv.id(), v.id());
      UNITTEST_ASSERT_EQUAL(sv.label(), v.label());
      UNITTEST_ASSERT_EQUAL(sv.color(), NOCOLOR);
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
  SimpleGraph kg(KV, KE);
  UNITTEST_ASSERT_EQUAL(kg.order(), KV.size());
  UNITTEST_ASSERT_EQUAL(kg.size(), KE.size());
  UNITTEST_ASSERT_FALSE(kg.null());
  UNITTEST_ASSERT_EQUAL(kg.mindeg(), 4);
  UNITTEST_ASSERT_EQUAL(kg.maxdeg(), 4);

  // Remove all of the vertices.
  const VertexNumbers out = {0, 4, 2, 3, 1};
  SimpleGraph sg(kg, out, EdgeNumbers());
  UNITTEST_ASSERT_EQUAL(sg.order(), 0);
  UNITTEST_ASSERT_EQUAL(sg.size(), 0);
  UNITTEST_ASSERT_TRUE(sg.null());
  UNITTEST_ASSERT_EQUAL(sg.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(sg.maxdeg(), 0);
}

TEST(contract_vertices) {
  // Start with a complete graph.
  SimpleGraph kg(KV, KE);
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
    UNITTEST_ASSERT_EQUAL(kg.outdeg(iv), d);
  }
  UNITTEST_ASSERT_EQUAL(kg.mindeg(), d);
  UNITTEST_ASSERT_EQUAL(kg.maxdeg(), d);

  // Contract two of the vertices.
  SimpleGraph cg1(kg, 0, 1);
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
    UNITTEST_ASSERT_EQUAL(cg1.outdeg(iv), d);
  }
  UNITTEST_ASSERT_EQUAL(cg1.mindeg(), d);
  UNITTEST_ASSERT_EQUAL(cg1.maxdeg(), d);

  Contracted c1 = {0, 1};
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg1.vertex(d).contracted(), c1);

  // Contract two more.
  SimpleGraph cg2(cg1, 3, 1);
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
    UNITTEST_ASSERT_EQUAL(cg2.outdeg(iv), d);
  }
  UNITTEST_ASSERT_EQUAL(cg2.mindeg(), d);
  UNITTEST_ASSERT_EQUAL(cg2.maxdeg(), d);

  c1.push_back(3);
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg2.vertex(d).contracted(), c1);

  // And once more.
  SimpleGraph cg3(cg2, 1, 0);
  UNITTEST_ASSERT_EQUAL(cg3.order(), cg2.order() - 1);
  UNITTEST_ASSERT_EQUAL(cg3.size(), cg2.size() - 2);
  UNITTEST_ASSERT_FALSE(cg3.empty());

  UNITTEST_ASSERT_EQUAL(cg3.vertex(0).id(), 6);
  UNITTEST_ASSERT_EQUAL(cg3.vertex(1).id(), 7);

  UNITTEST_ASSERT_EQUAL(cg3.edge(0).from(), 6);
  UNITTEST_ASSERT_EQUAL(cg3.edge(0).to(), 7);

  d = cg3.order() - 1;
  for (VertexNumber iv = 0; iv < cg3.order(); ++iv) {
    UNITTEST_ASSERT_EQUAL(cg3.outdeg(iv), d);
  }
  UNITTEST_ASSERT_EQUAL(cg3.mindeg(), d);
  UNITTEST_ASSERT_EQUAL(cg3.maxdeg(), d);

  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg3.vertex(0).contracted(), c1);
  Contracted c2 = {4, 2};
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg3.vertex(1).contracted(), c2);

  // And finally down to one vertex.
  SimpleGraph cg4(cg3, 0, 1);
  UNITTEST_ASSERT_EQUAL(cg4.order(), 1);
  UNITTEST_ASSERT_EQUAL(cg4.size(), 0);
  UNITTEST_ASSERT_TRUE(cg4.empty());

  UNITTEST_ASSERT_EQUAL(cg4.vertex(0).id(), 8);
  UNITTEST_ASSERT_EQUAL(cg4.outdeg(0), 0);
  UNITTEST_ASSERT_EQUAL(cg4.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(cg4.maxdeg(), 0);

  c1.insert(c1.end(), c2.cbegin(), c2.cend());
  UNITTEST_ASSERT_EQUAL_CONTAINERS(cg4.vertex(0).contracted(), c1);

  // Trying to contract a single vertex throws an exception.
  UNITTEST_ASSERT_THROW(SameVertexContractError, [&](){ SimpleGraph cg5(cg4, 0, 0); });
}
