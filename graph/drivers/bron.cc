// Executes the quick Bron-1 algorithm on a sequence of random graphs for a given order and edge probability.

#include <sstream>

#include "csv_sample_fields.h"
#include "csv_file.h"
#include "random_graph.h"
#include "bron1.h"
#include "bron2.h"

using namespace cavcom::utility;
using namespace cavcom::graph;

using BronVersion = Bron2;

class Statistics {
 public:
  Statistics(void) : order("n"), eprob("p"), edges("m"),
                     time("time"), calls("calls"), depth("depth"), cliques("cliques") {}
  CSVDatumField<VertexNumber> order;
  CSVDatumField<uint> eprob;
  CSVSampleFields<uint> edges;
  CSVSampleFields<double> time;
  CSVSampleFields<ullong> calls;
  CSVSampleFields<ullong> depth;
  CSVSampleFields<ullong> cliques;

  void add_fields(CSVFile *csv) {
    csv->add_field(&order);
    csv->add_field(&eprob);
    edges.add_fields(csv);
    time.add_fields(csv);
    calls.add_fields(csv);
    depth.add_fields(csv);
    cliques.add_fields(csv);
  }

  void gather_stats(VertexNumber n, uint p, const Bron &b1) {
    order.datum().value(n);
    eprob.datum().value(p);
    edges.add_data(b1.graph().size());
    std::chrono::duration<double> dt = b1.duration();
    time.add_data(dt.count());
    calls.add_data(b1.calls());
    depth.add_data(b1.maxdepth());
    cliques.add_data(b1.cliques().size());
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
        raw_data.gather_stats(n, ipct, bron);
        summary_data.gather_stats(n, ipct, bron);
        raw_file.write_data();
        raw_file.close();
      }

      summary_file.write_data();
      summary_file.close();
    }
  }
}
