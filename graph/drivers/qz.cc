// Executes the quick Zykov algorithm on a sequence of random graphs for a given order and edge probability.

#include <sstream>

#include "csv_sample_fields.h"
#include "csv_hit_counter_fields.h"
#include "csv_file.h"
#include "random_graph.h"
#include "quick_zykov.h"

using namespace cavcom::utility;
using namespace cavcom::graph;

class Statistics {
 public:
  Statistics(void) : order("n"), eprob("p"), edges("m"),
                     time("time"), steps("steps"), calls("calls"), depth("depth"),
                     lower_bound("lower_bound"), upper_bound("upper_bound"), number("number"), match("match"),
                     edge_threshold("edge_threshold"),
                     small_degree("small_degree"),
                     neighborhood_subset("neighborhood_subset"),
                     common_neighbors("common_neighbors"),
                     bounding("bounding") {}
  CSVDatumField<VertexNumber> order;
  CSVDatumField<uint> eprob;
  CSVSampleFields<uint> edges;
  CSVSampleFields<double> time;
  CSVSampleFields<ullong> steps;
  CSVSampleFields<ullong> calls;
  CSVSampleFields<ullong> depth;
  CSVSampleFields<ullong> lower_bound;
  CSVSampleFields<ullong> upper_bound;
  CSVSampleFields<ullong> number;
  CSVHitCounterFields match;
  CSVHitCounterFields edge_threshold;
  CSVHitCounterFields small_degree;
  CSVHitCounterFields neighborhood_subset;
  CSVHitCounterFields common_neighbors;
  CSVHitCounterFields bounding;

  void add_fields(CSVFile *csv) {
    csv->add_field(&order);
    csv->add_field(&eprob);
    edges.add_fields(csv);
    time.add_fields(csv);
    steps.add_fields(csv);
    calls.add_fields(csv);
    depth.add_fields(csv);
    lower_bound.add_fields(csv);
    upper_bound.add_fields(csv);
    number.add_fields(csv);
    match.add_fields(csv);
    edge_threshold.add_fields(csv);
    small_degree.add_fields(csv);
    neighborhood_subset.add_fields(csv);
    common_neighbors.add_fields(csv);
    bounding.add_fields(csv);
  }

  void gather_stats(VertexNumber n, uint p, const QuickZykov &qz) {
    order.datum().value(n);
    eprob.datum().value(p);
    edges.add_data(qz.graph().size());
    std::chrono::duration<double> dt = qz.duration();
    time.add_data(dt.count());
    steps.add_data(qz.steps());
    calls.add_data(qz.calls());
    depth.add_data(qz.maxdepth());
    lower_bound.add_data(qz.kmin());
    upper_bound.add_data(qz.kmax());
    number.add_data(qz.number());
    match.add_data(1, (qz.kmin() == qz.kmax()) ? 1 : 0);
    edge_threshold.add_data(qz.edge_threshold_tries(), qz.edge_threshold_hits());
    small_degree.add_data(qz.small_degree_tries(), qz.small_degree_hits());
    neighborhood_subset.add_data(qz.neighborhood_subset_tries(), qz.neighborhood_subset_hits());
    common_neighbors.add_data(qz.common_neighbors_tries(), qz.common_neighbors_hits());
    bounding.add_data(qz.bounding_tries(), qz.bounding_hits());
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
static constexpr VertexNumber N_END = 30;
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

      uint ntrials = (n < 20) ? TRIALS : TRIALS/10;
      for (uint itrial = 0; itrial < ntrials; ++itrial) {
        raw_file.reset_data();
        RandomGraph rg(n, ipct/100.0);
        QuickZykov qz(rg);
        qz.execute();
        raw_data.gather_stats(n, ipct, qz);
        summary_data.gather_stats(n, ipct, qz);
        raw_file.write_data();
        raw_file.close();
      }

      summary_file.write_data();
      summary_file.close();
    }
  }
}
