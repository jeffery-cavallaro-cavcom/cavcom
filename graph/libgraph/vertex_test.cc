// Creates and mutates vertices.

#include <libunittest/all.hpp>

#include "vertex.h"

using namespace cavcom::graph;

static void check_vertex(const Vertex &vertex, VertexID id,
                         const Label &label = Label(), Color color = BLACK,
                         Dimension xpos = 0.0, Dimension ypos = 0.0) {
  UNITTEST_ASSERT_EQUAL(vertex.id(), id);
  UNITTEST_ASSERT_EQUAL(vertex.label(), label);
  UNITTEST_ASSERT_EQUAL(vertex.color(), color);
  UNITTEST_ASSERT_TRUE(vertex.contracted().empty());
  UNITTEST_ASSERT_EQUAL(vertex.xpos(), xpos);
  UNITTEST_ASSERT_EQUAL(vertex.ypos(), ypos);
}

static void check_vertex(const Vertex &vertex, VertexID id, const VertexValues &values) {
  check_vertex(vertex, id, values.label, values.color, values.xpos, values.ypos);
}

static const VertexID ID = 3;
static const VertexValues VALUES = {"a vertex", 2, 4.5, -4.5};

TEST(create_vertex_no_attributes) {
  Vertex vertex(ID);
  check_vertex(vertex, ID);
}

TEST(create_vertex) {
  Vertex vertex(ID, VALUES);
  check_vertex(vertex, ID, VALUES);
}

TEST(set_vertex_attributes) {
  Vertex vertex(ID, VALUES.label);
  check_vertex(vertex, ID, VALUES.label);

  vertex.color(VALUES.color);
  vertex.xpos(VALUES.xpos);
  vertex.ypos(VALUES.ypos);

  check_vertex(vertex, ID, VALUES);
}

TEST(copy_vertex_no_attributes) {
  Vertex vertex1(ID);
  check_vertex(vertex1, ID);

  Vertex vertex2(vertex1);
  check_vertex(vertex2, ID);
}

TEST(copy_vertex) {
  Vertex vertex1(ID, VALUES);
  check_vertex(vertex1, ID, VALUES);

  Vertex vertex2(vertex1);
  check_vertex(vertex2, ID, VALUES);
}
