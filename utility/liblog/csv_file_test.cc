// Creates and writes to a CSV file.

#include <fstream>
#include <filesystem>

#include <libunittest/all.hpp>

#include "datum.h"
#include "sum.h"
#include "percent.h"
#include "csv_file.h"

using namespace cavcom::utility;

static const std::filesystem::path PATH = "/tmp/test.csv";

struct Fields {
  Fields(void) : d_letter("letter"), d_count("count"), d_sum("sum"), d_percent("percent"),
                 f_letter(&d_letter), f_count(&d_count), f_sum(&d_sum), f_percent(&d_percent) {}

  Datum<char> d_letter;
  Counter d_count;
  Sum<double> d_sum;
  Percent d_percent;

  CSVDatum<Datum<char>> f_letter;
  CSVDatum<Counter> f_count;
  CSVDatum<Sum<double>> f_sum;
  CSVDatum<Percent> f_percent;

  void set_data(char c, uint count, double sum, double percent) {
    d_letter.value(c);
    d_count.add(count);
    d_sum.add(sum);
    d_percent.value(percent);
  }
};

static Fields fields;

static void add_fields(CSVFile *csv) {
  csv->add_field(&fields.f_letter);
  csv->add_field(&fields.f_count);
  csv->add_field(&fields.f_sum);
  csv->add_field(&fields.f_percent);
}

static std::string make_path() {
  std::filesystem::path path = std::filesystem::temp_directory_path();
  path /= PATH;
  return path;
}

static const std::string EXPECTED = "letter,count,sum,percent\nA,1,2.5,50\nB,4,3.75,56.8\n";

TEST(create_csv_file) {
  CSVFile csv(make_path());
  add_fields(&csv);
  csv.write_header();

  fields.set_data('A', 1, 2.5, 0.5);
  csv.write_data();

  fields.set_data('B', 3, 1.25, 0.56789);
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
