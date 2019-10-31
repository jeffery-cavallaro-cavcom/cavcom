// Creates and mutates simple graphs.

#include <libunittest/all.hpp>

#include "edge.h"

using namespace cavcom::graph;

static void check_edge(const Edge &edge, VertexID from, VertexID to,
                       const Label &label = Label(), Color color = BLACK, Weight weight = FREE) {
  UNITTEST_ASSERT_EQUAL(edge.from(), from);
  UNITTEST_ASSERT_EQUAL(edge.to(), to);
  UNITTEST_ASSERT_EQUAL(edge.label(), label);
  UNITTEST_ASSERT_EQUAL(edge.color(), color);
  UNITTEST_ASSERT_EQUAL(edge.weight(), weight);
}

TEST(create_edge_no_attributes) {
  constexpr VertexID FROM = 1;
  constexpr VertexID TO = 2;

  Edge edge(FROM, TO);
  check_edge(edge, FROM, TO);
}

TEST(create_edge) {
  constexpr VertexID FROM = 3;
  constexpr VertexID TO = 6;
  const Label LABEL = "an edge";
  constexpr Color COLOR = 5;
  constexpr Weight WEIGHT = 1.0;

  Edge edge(FROM, TO, LABEL, COLOR, WEIGHT);
  check_edge(edge, FROM, TO, LABEL, COLOR, WEIGHT);
}

TEST(set_edge_attributes) {
  constexpr VertexID FROM = 3;
  constexpr VertexID TO = 6;
  const Label LABEL = "hello";
  constexpr Color COLOR = 2;
  constexpr Weight WEIGHT = 2.5;

  Edge edge(FROM, TO);
  check_edge(edge, FROM, TO);

  edge.label(LABEL);
  edge.color(COLOR);
  edge.weight(WEIGHT);
  check_edge(edge, FROM, TO, LABEL, COLOR, WEIGHT);
}

TEST(copy_edge_no_attributes) {
  constexpr VertexID FROM = 0;
  constexpr VertexID TO = 10;

  Edge edge1(FROM, TO);
  check_edge(edge1, FROM, TO);

  Edge edge2(edge1);
  check_edge(edge2, FROM, TO);
}

TEST(copy_edge) {
  constexpr VertexID FROM = 5;
  constexpr VertexID TO = 2;
  const Label LABEL = "world";
  constexpr Color COLOR = 4;
  constexpr Weight WEIGHT = 5.0;

  Edge edge1(FROM, TO, LABEL, COLOR, WEIGHT);
  check_edge(edge1, FROM, TO, LABEL, COLOR, WEIGHT);

  Edge edge2(edge1);
  check_edge(edge2, FROM, TO, LABEL, COLOR, WEIGHT);
}
