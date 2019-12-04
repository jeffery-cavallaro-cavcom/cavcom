// Executes the Edwards Elphick algorithm on a sequence of random graphs for a given order and edge probability.

#include <sstream>

#include "csv_sample_fields.h"
#include "csv_file.h"
#include "random_graph.h"
#include "clique_edwards.h"
#include "bron2.h"

using namespace cavcom::utility;
using namespace cavcom::graph;

using BronVersion = Bron2;

static const bool SMART = false;

class Statistics {
 public:
  Statistics(void) : order("n"), eprob("p"), edges("m"),
                     time("time"), steps("steps"), found("found"), actual("actual"), differ("differ") {}
  CSVDatumField<VertexNumber> order;
  CSVDatumField<uint> eprob;
  CSVSampleFields<uint> edges;
  CSVSampleFields<double> time;
  CSVSampleFields<ullong> steps;
  CSVSampleFields<Degree> found;
  CSVSampleFields<Degree> actual;
  CSVSampleFields<Degree> differ;

  void add_fields(CSVFile *csv) {
    csv->add_field(&order);
    csv->add_field(&eprob);
    edges.add_fields(csv);
    time.add_fields(csv);
    steps.add_fields(csv);
    found.add_fields(csv);
    actual.add_fields(csv);
    differ.add_fields(csv);
  }

  void gather_stats(VertexNumber n, uint p, Degree cn, const CliqueEdwards &ce) {
    order.datum().value(n);
    eprob.datum().value(p);
    edges.add_data(ce.graph().size());
    std::chrono::duration<double> dt = ce.duration();
    time.add_data(dt.count());
    steps.add_data(ce.steps());
    found.add_data(ce.number());
    actual.add_data(cn);
    differ.add_data(cn - ce.number());
  }
};

static std::string make_summary_filename(VertexNumber n) {
  std::ostringstream name;
  name << "data/summary_" << n << ".dat";
  return name.str();
}

static std::string make_raw_filename(VertexNumber n, uint ipct) {
  std::ostringstream name;
  name << "data/raw_" << n << "_" << ipct << ".dat";
  return name.str();
}

static constexpr uint TRIALS = 1000;

static constexpr VertexNumber N_START = 5;
static constexpr VertexNumber N_END = 50;
static constexpr VertexNumber N_INCR = 1;

static constexpr uint P_START = 10;
static constexpr uint P_END = 90;
static constexpr uint P_INCR = 10;

int main(int argc, char *argv[]) {
  for (VertexNumber n = N_START; n <= N_END; n += N_INCR) {
    Statistics summary_data;
    CSVFile summary_file(make_summary_filename(n));
    summary_data.add_fields(&summary_file);
    summary_file.open(true);
    summary_file.write_header();
    summary_file.close();

    for (uint ipct = P_START; ipct <= P_END; ipct += P_INCR) {
      summary_file.reset_data();

      Statistics raw_data;
      CSVFile raw_file(make_raw_filename(n, ipct));
      raw_data.add_fields(&raw_file);
      raw_file.open(true);
      raw_file.write_header();
      raw_file.close();

      for (uint itrial = 0; itrial < TRIALS; ++itrial) {
        raw_file.reset_data();
        RandomGraph rg(n, ipct/100.0);
        BronVersion bron(rg);
        bron.execute();
        CliqueEdwards ce(rg, SMART);
        ce.execute();
        raw_data.gather_stats(n, ipct, bron.number(), ce);
        summary_data.gather_stats(n, ipct, bron.number(), ce);
        raw_file.write_data();
        raw_file.close();
      }

      summary_file.write_data();
      summary_file.close();
    }
  }
}
