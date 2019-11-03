// Creates and exercises a vertex table.

#include <vector>

#include <libunittest/all.hpp>

#include "vertices.h"

using namespace cavcom::graph;

static void check_vertex(const Vertex &vertex, VertexID id, const VertexValues &values) {
  UNITTEST_ASSERT_EQUAL(vertex.id(), id);
  UNITTEST_ASSERT_EQUAL(vertex.label(), values.label);
  UNITTEST_ASSERT_EQUAL(vertex.color(), values.color);
  UNITTEST_ASSERT_TRUE(vertex.contracted().empty());
  UNITTEST_ASSERT_EQUAL(vertex.xpos(), values.xpos);
  UNITTEST_ASSERT_EQUAL(vertex.ypos(), values.ypos);
}

TEST(create_empty_table) {
  Vertices vertices;
  UNITTEST_ASSERT_EQUAL(vertices.size(), 0);
  UNITTEST_ASSERT_THROW(std::out_of_range, [&](){ vertices[0]; });
  VertexNumber found;
  UNITTEST_ASSERT_FALSE(vertices.find(0, &found));
  UNITTEST_ASSERT_FALSE(vertices.find("xxx", &found));
}

static const VertexValuesList VALUES = {{Label(), 0, 0, 0},
                                        {"vertex-1", 5, 1, -1},
                                        {"vertex-2", 1, 0, 3},
                                        {Label(), 2, -3, 0},
                                        {"vertex-4", 1, 1.5, 2},
                                        {"vertex-5", 0, -2.5, 2.5},
                                        {"vertex-6", 0, 1.5},
                                        {Label(), 3, -2, -2},
                                        {Label(), 0, -3, 3.5},
                                        {"vertex-9", 0, 5, 0}};

static constexpr VertexNumber UNLABELED = 3;
static constexpr VertexNumber LABELED = 6;
static const Label GOOD = "good";

static void add_vertices(Vertices *vertices) {
  vertices->add(VALUES);
  VertexValuesList::size_type n = VALUES.size();
  for (VertexValuesList::size_type iv = 0; iv < n; ++iv) {
    const VertexValues &v = VALUES[iv];
    VertexNumber number = 0;
    UNITTEST_ASSERT_TRUE(vertices->find(iv, &number));
    UNITTEST_ASSERT_EQUAL(number, iv);
    if (!v.label.empty()) {
      number = 0;
      UNITTEST_ASSERT_TRUE(vertices->find(v.label, &number));
      UNITTEST_ASSERT_EQUAL(number, iv);
    }
    check_vertex((*vertices)[iv], iv, v);
  }
  UNITTEST_ASSERT_EQUAL(vertices->size(), VALUES.size());
}

TEST(add_and_find_vertices) {
  Vertices vertices;
  add_vertices(&vertices);
}

TEST(unlabeled_to_unlabeled) {
  Vertices vertices;
  add_vertices(&vertices);

  Vertex &vertex = vertices[UNLABELED];
  UNITTEST_ASSERT_TRUE(vertex.label().empty());

  vertices.label(UNLABELED);
  UNITTEST_ASSERT_TRUE(vertex.label().empty());
}


TEST(unlabeled_to_labeled) {
  Vertices vertices;
  add_vertices(&vertices);

  Vertex &vertex = vertices[UNLABELED];
  UNITTEST_ASSERT_TRUE(vertex.label().empty());
  VertexNumber found;
  UNITTEST_ASSERT_FALSE(vertices.find(GOOD, &found));

  vertices.label(UNLABELED, GOOD);
  UNITTEST_ASSERT_EQUAL(vertex.label(), GOOD);
  UNITTEST_ASSERT_TRUE(vertices.find(GOOD, &found));
  UNITTEST_ASSERT_EQUAL(found, UNLABELED);
}

TEST(labeled_to_unlabeled) {
  Vertices vertices;
  add_vertices(&vertices);
  Label old = VALUES[LABELED].label;

  Vertex &vertex = vertices[LABELED];
  UNITTEST_ASSERT_EQUAL(vertex.label(), old);
  VertexNumber found;
  UNITTEST_ASSERT_TRUE(vertices.find(old, &found));
  UNITTEST_ASSERT_EQUAL(found, LABELED);

  vertices.label(LABELED);
  UNITTEST_ASSERT_TRUE(vertex.label().empty());
  UNITTEST_ASSERT_FALSE(vertices.find(old, &found));
}

TEST(labeled_to_labeled) {
  Vertices vertices;
  add_vertices(&vertices);
  Label old = VALUES[LABELED].label;

  Vertex &vertex = vertices[LABELED];
  UNITTEST_ASSERT_EQUAL(vertex.label(), old);
  VertexNumber found;
  UNITTEST_ASSERT_TRUE(vertices.find(old, &found));
  UNITTEST_ASSERT_EQUAL(found, LABELED);
  UNITTEST_ASSERT_FALSE(vertices.find(GOOD, &found));

  vertices.label(LABELED, GOOD);
  UNITTEST_ASSERT_EQUAL(vertex.label(), GOOD);
  UNITTEST_ASSERT_TRUE(vertices.find(GOOD, &found));
  UNITTEST_ASSERT_EQUAL(found, LABELED);
  UNITTEST_ASSERT_FALSE(vertices.find(old, &found));
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
}

TEST(reuse_label_on_new) {
  Vertices vertices;
  add_vertices(&vertices);
  Label label = vertices[LABELED].label();
  VertexNumber next = vertices.size();

  vertices.label(LABELED);
  vertices.add(label);
  VertexNumber found;
  UNITTEST_ASSERT_TRUE(vertices.find(label, &found));
  UNITTEST_ASSERT_EQUAL(found, next);

  UNITTEST_ASSERT_EQUAL(vertices.size(), VALUES.size() + 1);
}

TEST(set_duplicate_label) {
  Vertices vertices;
  add_vertices(&vertices);
  Label label = vertices[LABELED].label();
  UNITTEST_ASSERT_THROW(Vertices::Labels::DuplicateLookupError, [&](){ vertices.label(UNLABELED, label); });
}

TEST(reuse_duplicate_label) {
  Vertices vertices;
  add_vertices(&vertices);
  VertexNumber n = vertices.size();
  Label label = vertices[LABELED].label();
  UNITTEST_ASSERT_THROW(Vertices::Labels::DuplicateLookupError, [&](){ vertices.add(label, 1); });
  UNITTEST_ASSERT_EQUAL(vertices.size(), n);

  UNITTEST_ASSERT_EQUAL(vertices.size(), VALUES.size());
}
