// Creates and exercises a vertex table.

#include <vector>

#include <libunittest/all.hpp>

#include "vertices.h"

using namespace cavcom::graph;

static void check_vertex(const Vertex &vertex, VertexID id, const Label &label = Label(), Color color = BLACK) {
  UNITTEST_ASSERT_EQUAL(vertex.id(), id);
  UNITTEST_ASSERT_EQUAL(vertex.label(), label);
  UNITTEST_ASSERT_EQUAL(vertex.color(), color);
  UNITTEST_ASSERT_TRUE(vertex.contracted().empty());
}

TEST(create_empty_table) {
  Vertices vertices;
  UNITTEST_ASSERT_EQUAL(vertices.size(), 0);
  UNITTEST_ASSERT_THROW(std::out_of_range, [&](){ vertices[0]; });
  VertexNumber found;
  UNITTEST_ASSERT_FALSE(vertices.find(0, &found));
  UNITTEST_ASSERT_FALSE(vertices.find("xxx", &found));
}

using TestValues = struct { Label label; Color color; };
using TestData = std::vector<TestValues>;

static const TestData TESTDATA = {{Label(), 0}, {"vertex-1", 5}, {"vertex-2", 1}, {Label(), 2}, {"vertex-4", 1},
                                  {"vertex-5", 0}, {"vertex-6", 3}, {Label(), 3}, {Label(), 0}, {"vertex-9", 0}};

static constexpr VertexNumber UNLABELED = 3;
static constexpr VertexNumber LABELED = 6;
static const Label GOOD = "good";

static void check_size(const Vertices &vertices) {
  UNITTEST_ASSERT_EQUAL(vertices.size(), TESTDATA.size());
}

static void add_vertices(Vertices *vertices) {
  TestData::size_type n = TESTDATA.size();
  for (TestData::size_type iv = 0; iv < n; ++iv) {
    const TestValues &v = TESTDATA[iv];
    vertices->add(v.label, v.color);
    VertexNumber number;
    UNITTEST_ASSERT_TRUE(vertices->find(iv, &number));
    check_vertex((*vertices)[iv], iv, v.label, v.color);
  }
  check_size(*vertices);
}

TEST(add_and_find_vertices) {
  Vertices vertices;
  add_vertices(&vertices);
}

TEST(unlabeled_to_unlabeled) {
  Vertices vertices;
  add_vertices(&vertices);
  Color color = TESTDATA[UNLABELED].color;

  Vertex &vertex = vertices[UNLABELED];
  check_vertex(vertex, UNLABELED, Label(), color);

  vertices.label(UNLABELED);
  check_vertex(vertex, UNLABELED, Label(), color);

  check_size(vertices);
}

TEST(unlabeled_to_labeled) {
  Vertices vertices;
  add_vertices(&vertices);
  Color color = TESTDATA[UNLABELED].color;

  Vertex &vertex = vertices[UNLABELED];
  check_vertex(vertex, UNLABELED, Label(), color);
  VertexNumber found;
  UNITTEST_ASSERT_FALSE(vertices.find(GOOD, &found));

  vertices.label(UNLABELED, GOOD);
  check_vertex(vertex, UNLABELED, GOOD, color);
  UNITTEST_ASSERT_TRUE(vertices.find(GOOD, &found));
  UNITTEST_ASSERT_EQUAL(found, UNLABELED);

  check_size(vertices);
}

TEST(labeled_to_unlabeled) {
  Vertices vertices;
  add_vertices(&vertices);
  Label old = TESTDATA[LABELED].label;
  Color color = TESTDATA[LABELED].color;

  Vertex &vertex = vertices[LABELED];
  check_vertex(vertex, LABELED, old, color);
  VertexNumber found;
  UNITTEST_ASSERT_TRUE(vertices.find(old, &found));
  UNITTEST_ASSERT_EQUAL(found, LABELED);

  vertices.label(LABELED);
  check_vertex(vertex, LABELED, Label(), color);
  UNITTEST_ASSERT_FALSE(vertices.find(old, &found));

  check_size(vertices);
}

TEST(labeled_to_labeled) {
  Vertices vertices;
  add_vertices(&vertices);
  Label old = TESTDATA[LABELED].label;
  Color color = TESTDATA[LABELED].color;

  Vertex &vertex = vertices[LABELED];
  check_vertex(vertex, LABELED, old, color);
  VertexNumber found;
  UNITTEST_ASSERT_TRUE(vertices.find(old, &found));
  UNITTEST_ASSERT_EQUAL(found, LABELED);
  UNITTEST_ASSERT_FALSE(vertices.find(GOOD, &found));

  vertices.label(LABELED, GOOD);
  check_vertex(vertex, LABELED, GOOD, color);
  UNITTEST_ASSERT_TRUE(vertices.find(GOOD, &found));
  UNITTEST_ASSERT_EQUAL(found, LABELED);
  UNITTEST_ASSERT_FALSE(vertices.find(old, &found));

  check_size(vertices);
}

TEST(reuse_label_on_existing) {
  Vertices vertices;
  add_vertices(&vertices);
  Label label = vertices[LABELED].label();

  vertices.label(LABELED);
  vertices.label(UNLABELED, label);
  VertexNumber found;
  UNITTEST_ASSERT_TRUE(vertices.find(label, &found));
  UNITTEST_ASSERT_EQUAL(found, UNLABELED);

  check_size(vertices);
}

TEST(reuse_label_on_new) {
  Vertices vertices;
  add_vertices(&vertices);
  Label label = vertices[LABELED].label();
  VertexNumber next = vertices.size();

  vertices.label(LABELED);
  vertices.add(label, BLACK);
  VertexNumber found;
  UNITTEST_ASSERT_TRUE(vertices.find(label, &found));
  UNITTEST_ASSERT_EQUAL(found, next);

  UNITTEST_ASSERT_EQUAL(vertices.size(), TESTDATA.size() + 1);
}

TEST(set_duplicate_label) {
  Vertices vertices;
  add_vertices(&vertices);
  Label label = vertices[LABELED].label();
  UNITTEST_ASSERT_THROW(Vertices::Labels::DuplicateLookupError, [&](){ vertices.label(UNLABELED, label); });

  check_size(vertices);
}

TEST(reuse_duplicate_label) {
  Vertices vertices;
  add_vertices(&vertices);
  VertexNumber n = vertices.size();
  Label label = vertices[LABELED].label();
  UNITTEST_ASSERT_THROW(Vertices::Labels::DuplicateLookupError, [&](){ vertices.add(label, 1); });
  UNITTEST_ASSERT_EQUAL(vertices.size(), n);

  check_size(vertices);
}
