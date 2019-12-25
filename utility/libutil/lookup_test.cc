// Exercises lookup tables with integer and string keys and values.

#include <string>
#include <vector>

#include <libunittest/all.hpp>

#include "lookup.h"

using StringToInt = cavcom::utility::Lookup<std::string, int>;

TEST(throw_notfound_exception) {
  const std::string NAME = "stuff";
  const std::string VALUE = "hello";
  const std::string MESSAGE = "Unknown stuff: hello";

  try {
    throw StringToInt::NotFoundLookupError(NAME, VALUE);
  }
  catch (const StringToInt::NotFoundLookupError &error) {
    UNITTEST_ASSERT_EQUAL(error.key(), VALUE);
    UNITTEST_ASSERT_EQUAL(error.what(), MESSAGE);
  }
}

TEST(throw_duplicate_exception) {
  const std::string NAME = "junk";
  const std::string VALUE = "world";
  const std::string MESSAGE = "Duplicate junk: world";

  try {
    throw StringToInt::DuplicateLookupError(NAME, VALUE);
  }
  catch (const StringToInt::DuplicateLookupError &error) {
    UNITTEST_ASSERT_EQUAL(error.key(), VALUE);
    UNITTEST_ASSERT_EQUAL(error.what(), MESSAGE);
  }
}

TEST(create_empty_table) {
  const std::string name = "one";

  StringToInt lookup(name);

  UNITTEST_ASSERT_EQUAL(lookup.size(), 0);
  UNITTEST_ASSERT_FALSE(lookup.find("hello"));
  UNITTEST_ASSERT_THROW(StringToInt::NotFoundLookupError, [&lookup](){ lookup.find("world", true); });
}

using TestValues = struct { std::string svalue; int ivalue; };
using TestData = std::vector<TestValues>;
using SizeType = TestData::size_type;

static const TestData TESTDATA = {{"zero", 0}, {"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5},
                                  {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}, {"ten", 10}};

static void add_values(StringToInt *table, SizeType first, SizeType last) {
  for (SizeType iv = first; iv < last; ++iv) {
    const TestValues &v = TESTDATA[iv];
    UNITTEST_ASSERT_TRUE(table->add(v.svalue, v.ivalue, true));
    UNITTEST_ASSERT_EQUAL(*table->find(v.svalue, true), v.ivalue);
  }
}

TEST(add_and_lookup) {
  StringToInt lookup;
  UNITTEST_ASSERT_EQUAL(lookup.size(), 0);

  // Add the test values in two chunks.
  SizeType n = TESTDATA.size();
  SizeType n2 = n/2;
  add_values(&lookup, 0, n2);
  UNITTEST_ASSERT_EQUAL(lookup.size(), n2);
  add_values(&lookup, n2, n);
  UNITTEST_ASSERT_EQUAL(lookup.size(), n);

  // Access randomly.
  unittest::random_value<TestData::size_type> picker(TESTDATA.size() - 1);
  for (uint i = 0; i < 100; ++i) {
    TestData::size_type iv = picker.get();
    const TestValues &v = TESTDATA[iv];
    UNITTEST_ASSERT_EQUAL(*lookup.find(v.svalue, true), v.ivalue);
  }
}

TEST(key_not_found) {
  const std::string BAD = "xxx";
  StringToInt lookup;
  add_values(&lookup, 0, TESTDATA.size());
  UNITTEST_ASSERT_FALSE(lookup.find(BAD));
  UNITTEST_ASSERT_THROW(StringToInt::NotFoundLookupError,
                        ([&lookup, &BAD](){ lookup.find(BAD, true); }));
}

TEST(duplicate_key) {
  SizeType n = TESTDATA.size();
  const TestValues &v = TESTDATA[n/2];
  StringToInt lookup;
  add_values(&lookup, 0, n);
  UNITTEST_ASSERT_FALSE(lookup.add(v.svalue, v.ivalue));
  UNITTEST_ASSERT_THROW(StringToInt::DuplicateLookupError,
                        ([&lookup, &v](){ lookup.add(v.svalue, v.ivalue, true); }));
}

TEST(remove_key) {
  SizeType n = TESTDATA.size();
  const TestValues &v = TESTDATA[n/2];
  StringToInt lookup;
  add_values(&lookup, 0, n);
  lookup.remove(v.svalue);
  UNITTEST_ASSERT_EQUAL(lookup.size(), n - 1);
  UNITTEST_ASSERT_FALSE(lookup.find(v.svalue));
  UNITTEST_ASSERT_TRUE(lookup.add(v.svalue, v.ivalue, true));
  UNITTEST_ASSERT_EQUAL(lookup.size(), n);
  UNITTEST_ASSERT_EQUAL(*lookup.find(v.svalue, true), v.ivalue);
}

TEST(clear_table) {
  SizeType n = TESTDATA.size();
  StringToInt lookup;
  add_values(&lookup, 0, n);
  lookup.clear();
  UNITTEST_ASSERT_EQUAL(lookup.size(), 0);
  for (SizeType iv = 0; iv < n; ++iv) {
    const TestValues &v = TESTDATA[iv];
    UNITTEST_ASSERT_FALSE(lookup.find(v.svalue));
  }
  add_values(&lookup, 0, n);
  UNITTEST_ASSERT_EQUAL(lookup.size(), n);
}
