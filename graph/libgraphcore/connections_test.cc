// Creates and exercises a connection matrix.

#include <libunittest/all.hpp>

#include "connections.h"
#include "errors.h"

using namespace cavcom::graph;

static constexpr VertexNumber ORDER = 10;
static constexpr VertexNumber MAXDEG = ORDER - 1;
static constexpr VertexNumber FROM = 0;
static constexpr VertexNumber TO = 5;
static constexpr VertexNumber EDGE1 = 3;
static constexpr VertexNumber EDGE2 = 4;

static void check_degrees(const Connections &cm, Degree inmin, Degree inmax, Degree outmin, Degree outmax) {
  UNITTEST_ASSERT_EQUAL(cm.minindeg(), inmin);
  UNITTEST_ASSERT_EQUAL(cm.maxindeg(), inmax);
  UNITTEST_ASSERT_EQUAL(cm.minoutdeg(), outmin);
  UNITTEST_ASSERT_EQUAL(cm.maxoutdeg(), outmax);
  UNITTEST_ASSERT_EQUAL(cm.mindeg(), cm.minoutdeg());
  UNITTEST_ASSERT_EQUAL(cm.maxdeg(), cm.maxoutdeg());
}

TEST(create_empty_matrix) {
  Connections cm(ORDER);
  UNITTEST_ASSERT_EQUAL(cm.size(), ORDER);
  check_degrees(cm, 0, 0, 0, 0);
  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_EQUAL(cm.indeg(iv), 0);
    UNITTEST_ASSERT_EQUAL(cm.outdeg(iv), 0);
    UNITTEST_ASSERT_EQUAL(cm.degree(iv), 0);
  }
  UNITTEST_ASSERT_THROW(std::out_of_range, [&cm](){ cm.indeg(ORDER); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&cm](){ cm.outdeg(ORDER); });
  UNITTEST_ASSERT_THROW(std::out_of_range, [&cm](){ cm.degree(ORDER); });
}

TEST(add_one_simple_edge) {
  Connections cm(ORDER);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, TO, EDGE1), 1);

  UNITTEST_ASSERT_TRUE(cm.adjacent(FROM, TO));
  const Edges &edges = cm.edges(FROM, TO);
  UNITTEST_ASSERT_EQUAL(edges.size(), 1);
  UNITTEST_ASSERT_EQUAL(edges[0], EDGE1);

  UNITTEST_ASSERT_TRUE(cm.adjacent(TO, FROM));
  const Edges &other_edges = cm.edges(TO, FROM);
  UNITTEST_ASSERT_EQUAL(other_edges.size(), 1);
  UNITTEST_ASSERT_EQUAL(other_edges[0], EDGE1);

  UNITTEST_ASSERT_EQUAL(cm.indeg(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.degree(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.indeg(TO), 1);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(TO), 1);
  UNITTEST_ASSERT_EQUAL(cm.degree(TO), 1);

  check_degrees(cm, 0, 1, 0, 1);
}

TEST(add_one_directed_edge) {
  Connections cm(ORDER, true);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, TO, EDGE1), 1);

  UNITTEST_ASSERT_TRUE(cm.adjacent(FROM, TO));
  const Edges &edges = cm.edges(FROM, TO);
  UNITTEST_ASSERT_EQUAL(edges.size(), 1);
  UNITTEST_ASSERT_EQUAL(edges[0], EDGE1);

  UNITTEST_ASSERT_FALSE(cm.adjacent(TO, FROM));

  UNITTEST_ASSERT_EQUAL(cm.indeg(FROM), 0);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.degree(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.indeg(TO), 1);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(TO), 0);
  UNITTEST_ASSERT_EQUAL(cm.degree(TO), 0);

  check_degrees(cm, 0, 1, 0, 1);
}

TEST(add_two_directed_edges) {
  Connections cm(ORDER, true);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, TO, EDGE1), 1);
  UNITTEST_ASSERT_EQUAL(cm.join(TO, FROM, EDGE2), 1);

  UNITTEST_ASSERT_TRUE(cm.adjacent(FROM, TO));
  const Edges &edges = cm.edges(FROM, TO);
  UNITTEST_ASSERT_EQUAL(edges.size(), 1);
  UNITTEST_ASSERT_EQUAL(edges[0], EDGE1);

  UNITTEST_ASSERT_TRUE(cm.adjacent(TO, FROM));
  const Edges &other_edges = cm.edges(TO, FROM);
  UNITTEST_ASSERT_EQUAL(other_edges.size(), 1);
  UNITTEST_ASSERT_EQUAL(other_edges[0], EDGE2);

  UNITTEST_ASSERT_EQUAL(cm.indeg(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.degree(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.indeg(TO), 1);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(TO), 1);
  UNITTEST_ASSERT_EQUAL(cm.degree(TO), 1);

  check_degrees(cm, 0, 1, 0, 1);
}

TEST(add_undirected_multiple_edges) {
  Connections cm(ORDER, false, true);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, TO, EDGE1), 1);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, TO, EDGE2), 2);

  UNITTEST_ASSERT_TRUE(cm.adjacent(FROM, TO));
  const Edges &edges = cm.edges(FROM, TO);
  UNITTEST_ASSERT_EQUAL(edges.size(), 2);
  UNITTEST_ASSERT_EQUAL(edges[0], EDGE1);
  UNITTEST_ASSERT_EQUAL(edges[1], EDGE2);

  UNITTEST_ASSERT_TRUE(cm.adjacent(TO, FROM));
  const Edges &other_edges = cm.edges(TO, FROM);
  UNITTEST_ASSERT_EQUAL(other_edges.size(), 2);
  UNITTEST_ASSERT_EQUAL(other_edges[0], EDGE1);
  UNITTEST_ASSERT_EQUAL(other_edges[1], EDGE2);

  UNITTEST_ASSERT_EQUAL(cm.indeg(FROM), 2);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(FROM), 2);
  UNITTEST_ASSERT_EQUAL(cm.degree(FROM), 2);
  UNITTEST_ASSERT_EQUAL(cm.indeg(TO), 2);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(TO), 2);
  UNITTEST_ASSERT_EQUAL(cm.degree(TO), 2);

  check_degrees(cm, 0, 2, 0, 2);
}

TEST(add_directed_multiple_edges) {
  Connections cm(ORDER, true, true);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, TO, EDGE1), 1);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, TO, EDGE2), 2);

  UNITTEST_ASSERT_TRUE(cm.adjacent(FROM, TO));
  const Edges &edges = cm.edges(FROM, TO);
  UNITTEST_ASSERT_EQUAL(edges.size(), 2);
  UNITTEST_ASSERT_EQUAL(edges[0], EDGE1);
  UNITTEST_ASSERT_EQUAL(edges[1], EDGE2);

  UNITTEST_ASSERT_FALSE(cm.adjacent(TO, FROM));

  UNITTEST_ASSERT_EQUAL(cm.indeg(FROM), 0);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(FROM), 2);
  UNITTEST_ASSERT_EQUAL(cm.degree(FROM), 2);
  UNITTEST_ASSERT_EQUAL(cm.indeg(TO), 2);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(TO), 0);
  UNITTEST_ASSERT_EQUAL(cm.degree(TO), 0);

  check_degrees(cm, 0, 2, 0, 2);
}

TEST(add_undirected_loop_edge) {
  Connections cm(ORDER, false, false, true);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, FROM, EDGE1), 1);

  UNITTEST_ASSERT_TRUE(cm.adjacent(FROM, FROM));
  const Edges &edges = cm.edges(FROM, FROM);
  UNITTEST_ASSERT_EQUAL(edges.size(), 1);
  UNITTEST_ASSERT_EQUAL(edges[0], EDGE1);

  UNITTEST_ASSERT_EQUAL(cm.indeg(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.degree(FROM), 1);

  check_degrees(cm, 0, 1, 0, 1);
}

TEST(add_directed_loop_edge) {
  Connections cm(ORDER, true, false, true);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, FROM, EDGE1), 1);

  UNITTEST_ASSERT_TRUE(cm.adjacent(FROM, FROM));
  const Edges &edges = cm.edges(FROM, FROM);
  UNITTEST_ASSERT_EQUAL(edges.size(), 1);
  UNITTEST_ASSERT_EQUAL(edges[0], EDGE1);

  UNITTEST_ASSERT_EQUAL(cm.indeg(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(FROM), 1);
  UNITTEST_ASSERT_EQUAL(cm.degree(FROM), 1);

  check_degrees(cm, 0, 1, 0, 1);
}

TEST(add_multiple_undirected_loop_edges) {
  Connections cm(ORDER, false, true, true);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, FROM, EDGE1), 1);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, FROM, EDGE2), 2);

  UNITTEST_ASSERT_TRUE(cm.adjacent(FROM, FROM));
  const Edges &edges = cm.edges(FROM, FROM);
  UNITTEST_ASSERT_EQUAL(edges.size(), 2);
  UNITTEST_ASSERT_EQUAL(edges[0], EDGE1);
  UNITTEST_ASSERT_EQUAL(edges[1], EDGE2);

  UNITTEST_ASSERT_EQUAL(cm.indeg(FROM), 2);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(FROM), 2);
  UNITTEST_ASSERT_EQUAL(cm.degree(FROM), 2);

  check_degrees(cm, 0, 2, 0, 2);
}

TEST(add_multiple_directed_loop_edges) {
  Connections cm(ORDER, true, true, true);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, FROM, EDGE1), 1);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, FROM, EDGE2), 2);

  UNITTEST_ASSERT_TRUE(cm.adjacent(FROM, FROM));
  const Edges &edges = cm.edges(FROM, FROM);
  UNITTEST_ASSERT_EQUAL(edges.size(), 2);
  UNITTEST_ASSERT_EQUAL(edges[0], EDGE1);
  UNITTEST_ASSERT_EQUAL(edges[1], EDGE2);

  UNITTEST_ASSERT_EQUAL(cm.indeg(FROM), 2);
  UNITTEST_ASSERT_EQUAL(cm.outdeg(FROM), 2);
  UNITTEST_ASSERT_EQUAL(cm.degree(FROM), 2);

  check_degrees(cm, 0, 2, 0, 2);
}

TEST(prevent_multiple_edges) {
  Connections cm(ORDER);
  UNITTEST_ASSERT_EQUAL(cm.join(FROM, TO, EDGE1), 1);
  UNITTEST_ASSERT_THROW(MultipleEdgeError, [&cm](){ cm.join(FROM, TO, EDGE2); });
}

TEST(prevent_loop_edges) {
  Connections cm(ORDER);
  UNITTEST_ASSERT_THROW(LoopEdgeError, [&cm](){ cm.join(FROM, FROM, EDGE1); });
}

TEST(make_a_path) {
  Connections cm(ORDER);
  EdgeNumber ie = 0;
  for (VertexNumber iv = 1; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_EQUAL(cm.join(iv, iv - 1, ie++), 1);
  }

  for (VertexNumber iv = 1; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_TRUE(cm.adjacent(iv, iv - 1));
  }
  UNITTEST_ASSERT_FALSE(cm.adjacent(0, ORDER - 1));

  UNITTEST_ASSERT_EQUAL(cm.degree(0), 1);
  for (VertexNumber iv = 1; iv < ORDER - 2; ++iv) {
    UNITTEST_ASSERT_EQUAL(cm.degree(iv), 2);
  }
  UNITTEST_ASSERT_EQUAL(cm.degree(ORDER - 1), 1);

  check_degrees(cm, 1, 2, 1, 2);
}

TEST(make_a_cycle) {
  Connections cm(ORDER);
  EdgeNumber ie = 0;
  for (VertexNumber iv = 1; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_EQUAL(cm.join(iv, iv - 1, ie++), 1);
  }
  UNITTEST_ASSERT_EQUAL(cm.join(0, ORDER - 1, ie++), 1);

  for (VertexNumber iv = 1; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_TRUE(cm.adjacent(iv, iv - 1));
  }
  UNITTEST_ASSERT_TRUE(cm.adjacent(0, ORDER - 1));

  for (VertexNumber iv = 0; iv < ORDER; ++iv) {
    UNITTEST_ASSERT_EQUAL(cm.degree(iv), 2);
  }

  check_degrees(cm, 2, 2, 2, 2);
}

TEST(make_a_complete) {
  Connections cm(ORDER);
  EdgeNumber ie = 0;
  for (VertexNumber i = 0; i < ORDER - 1; ++i) {
    for (VertexNumber j = i + 1; j < ORDER; ++j) {
      cm.join(i, j, ie++);
    }
  }

  for (VertexNumber i = 0; i < ORDER - 1; ++i) {
    for (VertexNumber j = 0; j < ORDER - 1; ++j) {
      if (i == j) {
        UNITTEST_ASSERT_FALSE(cm.adjacent(i, j));
      } else {
        UNITTEST_ASSERT_TRUE(cm.adjacent(i, j));
      }
    }
  }

  check_degrees(cm, MAXDEG, MAXDEG, MAXDEG, MAXDEG);
}
