// Creates and exercises a vertex table.

#include <vector>

#include <libunittest/all.hpp>

#include "vertices.h"

using namespace cavcom::graph;

static void check_vertex(const Vertex &vertex, VertexID id, const Label &label = Label(), Color color = BLACK) {
  UNITTEST_ASSERT_EQUAL(vertex.id(), id);
  UNITTEST_ASSERT_EQUAL(vertex.label(), label);
  UNITTEST_ASSERT_EQUAL(vertex.color(), color);
  UNITTEST_ASSERT_EQUAL(vertex.indeg(), 0);
  UNITTEST_ASSERT_EQUAL(vertex.outdeg(), 0);
  UNITTEST_ASSERT_EQUAL(vertex.degree(), 0);
  UNITTEST_ASSERT_TRUE(vertex.contracted().empty());
}

TEST(create_empty_table) {
  Vertices vertices;
  UNITTEST_ASSERT_EQUAL(vertices.size(), 0);
  VertexNumber found;
  UNITTEST_ASSERT_FALSE(vertices.find(0, &found));
  UNITTEST_ASSERT_FALSE(vertices.find("xxx", &found));
}

using TestValues = struct { Label label; Color color; };
using TestData = std::vector<TestValues>;

static const TestData TESTDATA = {{Label(), 0}, {"vertex-1", 5}};

static void add_vertices(Vertices *vertices) {
  TestData::size_type n = TESTDATA.size();
  for (TestData::size_type iv = 0; iv < n; ++iv) {
    const TestValues &v = TESTDATA[iv];
    vertices->add(v.label, v.color);
    VertexNumber number;
    UNITTEST_ASSERT_TRUE(vertices->find(iv, &number));
    check_vertex((*vertices)[iv], iv, v.label, v.color);
  }
}

TEST(add_and_find_vertices) {
  Vertices vertices;
  add_vertices(&vertices);
  UNITTEST_ASSERT_EQUAL(vertices.size(), TESTDATA.size());
}
