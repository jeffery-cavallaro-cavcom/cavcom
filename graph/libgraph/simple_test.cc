// Creates and mutates simple graphs.

#include <stdexcept>

#include <libunittest/all.hpp>

#include "simple.h"

using cavcom::graph::SimpleGraph;

TEST(create_null_graph) {
  SimpleGraph g;

  UNITTEST_ASSERT_EQUAL(g.order(), 0);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);
  UNITTEST_ASSERT_EQUAL(g.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), 0);
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g[0]; });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.vertex(0); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.edges(0, 0); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.edge(0, 0, 0); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.adjacent(0, 0); });
}

static void CheckIsolatedVertex(const SimpleGraph &g, SimpleGraph::VertexNumber iv) {
  const SimpleGraph::Vertex &v = g[iv];
  UNITTEST_ASSERT_EQUAL(v.id(), iv);
  UNITTEST_ASSERT_TRUE(v.label().empty());
  UNITTEST_ASSERT_EQUAL(v.color(), SimpleGraph::BLACK);
  UNITTEST_ASSERT_EQUAL(v.indeg(), 0);
  UNITTEST_ASSERT_EQUAL(v.outdeg(), 0);
}

TEST(create_trivial_graph) {
  SimpleGraph g;
  g.add_vertices(1);

  UNITTEST_ASSERT_EQUAL(g.order(), 1);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);
  CheckIsolatedVertex(g, 0);

  UNITTEST_ASSERT_EQUAL(g.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), 0);
  UNITTEST_ASSERT_FALSE(g.adjacent(0,0));
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.edges(0, 1); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.edge(0, 0, 0); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.adjacent(0, 1); });
}

TEST(create_unlabeled_graph) {
  constexpr SimpleGraph::VertexNumber n = 5;

  SimpleGraph g;
  g.add_vertices(n);

  UNITTEST_ASSERT_EQUAL(g.order(), n);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);

  for (SimpleGraph::VertexNumber iv = 0; iv < n; ++iv) {
    CheckIsolatedVertex(g, iv);
  }

  UNITTEST_ASSERT_EQUAL(g.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), 0);
  for (SimpleGraph::VertexNumber i = 0; i < n; ++i) {
    for (SimpleGraph::VertexNumber j = 0; j < n; ++j) {
      UNITTEST_ASSERT_FALSE(g.adjacent(i,j));
    }
  }
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.adjacent(0, n); });
}

TEST(add_isolated_vertices) {
  constexpr SimpleGraph::VertexNumber n1 = 1;
  constexpr SimpleGraph::VertexNumber n2 = 5;
  constexpr SimpleGraph::VertexNumber n3 = 2;
  constexpr SimpleGraph::VertexNumber n = n1 + n2 + n3;

  SimpleGraph g;
  g.add_vertices(n1);
  g.add_vertices(n2);
  g.add_vertices(n3);

  UNITTEST_ASSERT_EQUAL(g.order(), n);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);

  for (SimpleGraph::VertexNumber iv = 0; iv < n; ++iv) {
    CheckIsolatedVertex(g, iv);
  }

  UNITTEST_ASSERT_EQUAL(g.mindeg(), 0);
  UNITTEST_ASSERT_EQUAL(g.maxdeg(), 0);
  for (SimpleGraph::VertexNumber i = 0; i < n; ++i) {
    for (SimpleGraph::VertexNumber j = 0; j < n; ++j) {
      UNITTEST_ASSERT_FALSE(g.adjacent(i,j));
    }
  }
  UNITTEST_ASSERT_THROW(std::out_of_range, [&g](){ return g.adjacent(0, n); });
}

TEST(make_labeled_graph) {
  const SimpleGraph::VertexValuesList VALUES1 = {{"a", 1}, {"b1", 2}, {"c23", 5}, {"d456", 0}, {"e9", 3}};
  const SimpleGraph::VertexValuesList VALUES2 = {{"hello", 0}, {"and", 1}, {"goodbye", 4}};

  SimpleGraph g;
  g.add_vertices(VALUES1);
  g.add_vertices(VALUES2);

  SimpleGraph::VertexNumber n1 = VALUES1.size();
  SimpleGraph::VertexNumber n2 = VALUES2.size();
  SimpleGraph::VertexNumber n = n1 + n2;

  UNITTEST_ASSERT_EQUAL(g.order(), n);
  UNITTEST_ASSERT_EQUAL(g.size(), 0);

  for (SimpleGraph::VertexNumber iv = 0; iv < n; ++iv) {
    const SimpleGraph::Vertex &v = g[iv];
    const SimpleGraph::VertexValues &a = (iv < n1) ? VALUES1[iv] : VALUES2[iv - n1];
    UNITTEST_ASSERT_EQUAL(v.id(), iv);
    UNITTEST_ASSERT_EQUAL(v.label(), a.label);
    UNITTEST_ASSERT_EQUAL(v.color(), a.color);
    UNITTEST_ASSERT_EQUAL(v.indeg(), 0);
    UNITTEST_ASSERT_EQUAL(v.outdeg(), 0);
  }
}
