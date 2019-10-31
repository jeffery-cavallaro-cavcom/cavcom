// Creates and mutates vertices.

#include <libunittest/all.hpp>

#include "vertex.h"

using namespace cavcom::graph;

static void check_vertex(const Vertex &vertex, const Label &label = Label(), Color color = BLACK) {
  UNITTEST_ASSERT_EQUAL(vertex.id(), 0);
  UNITTEST_ASSERT_EQUAL(vertex.label(), label);
  UNITTEST_ASSERT_EQUAL(vertex.color(), color);
  UNITTEST_ASSERT_EQUAL(vertex.indeg(), 0);
  UNITTEST_ASSERT_EQUAL(vertex.outdeg(), 0);
  UNITTEST_ASSERT_EQUAL(vertex.degree(), 0);
  UNITTEST_ASSERT_TRUE(vertex.contracted().empty());
}

TEST(create_vertex_no_attributes) {
  Vertex vertex;
  check_vertex(vertex);
}

TEST(create_vertex) {
  const Label LABEL = "a vertex";
  constexpr Color COLOR = 5;

  Vertex vertex(LABEL, COLOR);
  check_vertex(vertex, LABEL, COLOR);
}

TEST(set_vertex_attributes) {
  constexpr Color COLOR = 2;

  Vertex vertex;
  check_vertex(vertex);

  vertex.color(COLOR);
  check_vertex(vertex, Label(), COLOR);
}

TEST(copy_vertex_no_attributes) {
  Vertex vertex1;
  check_vertex(vertex1);

  Vertex vertex2(vertex1);
  check_vertex(vertex2);
}

TEST(copy_vertex) {
  const Label LABEL = "world";
  constexpr Color COLOR = 4;

  Vertex vertex1(LABEL, COLOR);
  check_vertex(vertex1, LABEL, COLOR);

  Vertex vertex2(vertex1);
  check_vertex(vertex2, LABEL, COLOR);
}
