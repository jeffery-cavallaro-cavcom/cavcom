// Creates and writes to a CSV file.

#include <fstream>
#include <filesystem>

#include <libunittest/all.hpp>

#include "sum.h"
#include "percent.h"
#include "csv_file.h"

using namespace cavcom::utility;

static const std::filesystem::path PATH = "/tmp/test.csv";

struct Fields {
  Fields(void) : letter("letter"), sum("sum"), count("count"), percent("percent") {}

  CSVDatumField<char> letter;
  CSVSumField<double> sum;
  CSVCounterField count;
  CSVPercentField percent;

  void set_data(char c_value, double sum_value, uint count_value, double percent_value) {
    letter.datum().value(c_value);
    sum.datum().add(sum_value);
    count.datum().add(count_value);
    percent.datum().value(percent_value);
  }
};

static Fields fields;

static void add_fields(CSVFile *csv) {
  csv->add_field(&fields.letter);
  csv->add_field(&fields.sum);
  csv->add_field(&fields.count);
  csv->add_field(&fields.percent);
}

static std::string make_path() {
  std::filesystem::path path = std::filesystem::temp_directory_path();
  path /= PATH;
  return path;
}

static const std::string EXPECTED = "letter,sum,count,percent\nA,2.5,1,50\nB,3.75,4,56.8\n";

TEST(create_csv_file) {
  CSVFile csv(make_path());
  add_fields(&csv);
  csv.write_header();

  fields.set_data('A', 2.5, 1, 0.5);
  csv.write_data();

  fields.set_data('B', 1.25, 3, 0.56789);
  csv.write_data();

  csv.close();

  UNITTEST_ASSERT_EQUAL(std::filesystem::file_size(PATH), 50);

  std::string contents;
  std::string next;
  std::ifstream in;
  in.open(PATH);
  while (std::getline(in, next)) {
    contents += next;
    contents += "\n";
  }
  in.close();
  UNITTEST_ASSERT_EQUAL(contents, EXPECTED);

  std::filesystem::remove(PATH);
}
