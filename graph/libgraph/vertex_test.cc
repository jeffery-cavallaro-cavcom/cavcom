// Creates and mutates vertices.

#include <libunittest/all.hpp>

#include "vertex.h"

using namespace cavcom::graph;

static void check_vertex(const Vertex &vertex, VertexID id, const Label &label = Label(), Color color = BLACK) {
  UNITTEST_ASSERT_EQUAL(vertex.id(), id);
  UNITTEST_ASSERT_EQUAL(vertex.label(), label);
  UNITTEST_ASSERT_EQUAL(vertex.color(), color);
  UNITTEST_ASSERT_TRUE(vertex.contracted().empty());
}

TEST(create_vertex_no_attributes) {
  constexpr VertexID ID = 0;

  Vertex vertex(ID);
  check_vertex(vertex, ID);
}

TEST(create_vertex) {
  constexpr VertexID ID = 42;
  const Label LABEL = "a vertex";
  constexpr Color COLOR = 5;

  Vertex vertex(ID, LABEL, COLOR);
  check_vertex(vertex, ID, LABEL, COLOR);
}

TEST(set_vertex_attributes) {
  constexpr VertexID ID = 1;
  constexpr Color COLOR = 2;

  Vertex vertex(ID);
  check_vertex(vertex, ID);

  vertex.color(COLOR);
  check_vertex(vertex, ID, Label(), COLOR);
}

TEST(copy_vertex_no_attributes) {
  constexpr VertexID ID = 123;
  Vertex vertex1(ID);
  check_vertex(vertex1, ID);

  Vertex vertex2(vertex1);
  check_vertex(vertex2, ID);
}

TEST(copy_vertex) {
  constexpr VertexID ID = 5;
  const Label LABEL = "world";
  constexpr Color COLOR = 4;

  Vertex vertex1(ID, LABEL, COLOR);
  check_vertex(vertex1, ID, LABEL, COLOR);

  Vertex vertex2(vertex1);
  check_vertex(vertex2, ID, LABEL, COLOR);
}
