// Sets and checks vertex attribute values.

#include <libunittest/all.hpp>

#include "vertex.h"

using cavcom::graph::Vertex;

static constexpr const char *TEST_LABEL = "label";
static constexpr Vertex::Color TEST_COLOR = 10;

static void check_inited(const Vertex &vertex) {
  UNITTEST_ASSERT_EQUAL(vertex.id(), 0);
  UNITTEST_ASSERT_EQUAL(vertex.label().empty(), true);
  UNITTEST_ASSERT_EQUAL(vertex.color(), Vertex::NOCOLOR);
}

static void check_values(const Vertex &vertex) {
  UNITTEST_ASSERT_EQUAL(vertex.id(), 0);
  UNITTEST_ASSERT_EQUAL(vertex.label(), TEST_LABEL);
  UNITTEST_ASSERT_EQUAL(vertex.color(), TEST_COLOR);
}

TEST(create_and_access_vertex) {
  UNITTEST_TESTINFO("Creates a vertex and checks attributes");

  // Check default values.
  Vertex vertex;
  check_inited(vertex);

  // Change and test the label attribute.
  vertex.label(TEST_LABEL);
  vertex.color(TEST_COLOR);
  check_values(vertex);
}

TEST(copy_vertex) {
  UNITTEST_TESTINFO("Copies vertices with and without set attributes");

  // Copy no attributes.
  Vertex v1;
  check_inited(v1);
  Vertex v2(v1);
  check_inited(v2);

  // Copy attributes.
  v1.label(TEST_LABEL);
  v1.color(TEST_COLOR);
  Vertex v3(v1);
  check_values(v3);
}
