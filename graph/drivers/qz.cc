// Executes the quick Zykov algorithm on a sequence of random graphs for a given order and edge probability.

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "random_graph.h"
#include "quick_zykov.h"

using namespace cavcom::graph;

template <typename T>
class Sample {
 public:
  explicit Sample(const std::string &name) : name_(name), count_(0), min_(0), max_(0), sum_(0), sum2_(0) {}

  const std::string &name(void) const { return name_; }
  T sum() const { return sum_; }

  void add(T value) {
    sum_ += value;
    sum2_ += (value*value);

    if (count_ <= 0) {
      min_ = value;
      max_ = value;
    } else {
      if (value < min_) min_ = value;
      if (value > max_) max_ = value;
    }

    ++count_;
  }

  void header(std::ostream *out) {
    *out << name_ << "_mean," << name_ << "_stddev," << name_ << "_min," << name_ << "_max";
  }

  void data(std::ostream *out) {
    double sample_mean = mean();
    double sample_stddev = stddev(sample_mean);
    *out << sample_mean << "," << sample_stddev << "," << min_ << "," << max_;
  }

 private:
  const std::string name_;
  ulong count_;
  T min_;
  T max_;
  T sum_;
  T sum2_;

  double mean(void) const { return sum_/count_; }

  double stddev(double mean) const {
    return sqrt((sum2_ - count_*mean*mean)/(count_ - 1.0));
  }
};

class HitCounter {
 public:
  HitCounter(const std::string &name) : name_(name), tries_(0), hits_(0) {}

  const std::string &name(void) const { return name_; }
  ulong tries(void) const { return tries_; }
  ulong hits(void) const { return hits_; }

  void add(ulong tries, ulong hits) {
    tries_ += tries;
    hits_ += hits;
  }

  void header(std::ostream *out) {
    *out << name_ << "_tries," << name_ << "_hits," << name_ << "_percent";
  }

  void data(std::ostream *out) {
    *out << tries_ << "," << hits_ << "," << percent();
  }

 private:
  const std::string name_;
  ulong tries_;
  ulong hits_;

  double percent(void) {
    if (tries_ == 0) return 0.0;
    double p = static_cast<double>(hits_)/tries_;
    ullong ip = 10000.0*p;
    return ip/100.0;
  }
};

class Statistics {
 public:
  Statistics(void) : time("time"), steps("steps"), calls("calls"), depth("depth"),
                     edge_threshold("edge_threshold"), small_degree("small_degree"),
                     neighborhood_subset("neighborhood_subset"), common_neighbors("common_neighbors") {}
  Sample<double> time;
  Sample<ullong> steps;
  Sample<ullong> calls;
  Sample<ullong> depth;
  HitCounter edge_threshold;
  HitCounter small_degree;
  HitCounter neighborhood_subset;
  HitCounter common_neighbors;

  void raw_header(std::ostream *out) {
    *out << "nodes,eprob,trial,"
         << time.name() << ","
         << steps.name() << ","
         << calls.name() << ","
         << depth.name() << ",";
    edge_threshold.header(out);
    *out << ",";
    small_degree.header(out);
    *out << ",";
    neighborhood_subset.header(out);
    *out << ",";
    common_neighbors.header(out);
    *out << std::endl;
  }

  void raw_data(VertexNumber nodes, uint eprob, uint trial, std::ostream *out) {
    *out << nodes << "," << eprob << "," << trial << ","
         << time.sum() << ","
         << steps.sum() << ","
         << calls.sum() << ","
         << depth.sum() << ",";
    edge_threshold.data(out);
    *out << ",";
    small_degree.data(out);
    *out << ",";
    neighborhood_subset.data(out);
    *out << ",";
    common_neighbors.data(out);
    *out << std::endl;
  }

  void summary_header(std::ostream *out) {
    *out << "nodes,eprob,";
    time.header(out);
    *out << ",";
    steps.header(out);
    *out << ",";
    calls.header(out);
    *out << ",";
    depth.header(out);
    *out << ",";
    edge_threshold.header(out);
    *out << ",";
    small_degree.header(out);
    *out << ",";
    neighborhood_subset.header(out);
    *out << ",";
    common_neighbors.header(out);
    *out << std::endl;
  }

  void gather_stats(const QuickZykov &qz) {
    std::chrono::duration<double> dt = qz.duration();
    time.add(dt.count());
    steps.add(qz.steps());
    calls.add(qz.calls());
    depth.add(qz.maxdepth());
    edge_threshold.add(qz.edge_threshold_tries(), qz.edge_threshold_hits());
    small_degree.add(qz.small_degree_tries(), qz.small_degree_hits());
    neighborhood_subset.add(qz.neighborhood_subset_tries(), qz.neighborhood_subset_hits());
    common_neighbors.add(qz.common_neighbors_tries(), qz.common_neighbors_hits());
  }

  void add_stats(const Statistics &from) {
    time.add(from.time.sum());
    steps.add(from.steps.sum());
    calls.add(from.calls.sum());
    depth.add(from.depth.sum());
    edge_threshold.add(from.edge_threshold.tries(), from.edge_threshold.hits());
    small_degree.add(from.small_degree.tries(), from.small_degree.hits());
    neighborhood_subset.add(from.neighborhood_subset.tries(), from.neighborhood_subset.hits());
    common_neighbors.add(from.common_neighbors.tries(), from.common_neighbors.hits());
  }
};

static constexpr uint TRIALS = 1000;

static void make_summary_file(void) {
  Statistics headers;
  std::ostringstream fn;
  fn << "data/summary.dat";
  std::ofstream fd(fn.str(), std::ofstream::out|std::ofstream::trunc);
  header.summary_header(&fd);
  fd.close();
}

static void make_raw_file(VertexNumber n, uint ipct, const Statistics &data) {
  std::ostringstream fn;
  fn << "data/raw_" << n << "_" << ipct << ".dat";
  std::ofstream fd(fn.str(), std::ofstream::out|std::ofstream::trunc);
  data.raw_header(&fd);
  fd.close();
}

int main(int argc, char *argv[]) {
  make_summary_file();

  for (VertexNumber n = 5; n <= 20; ++n) {
    for (uint ipct = 10; ipct <= 90; ipct += 10) {
      Statistics data;

      
      
      for (uint itrial = 0; itrial < TRIALS; ++itrial) {
        RandomGraph rg(n, ipct/100.0);
        QuickZykov qz(rg);
        //        qz.execute();
        Statistics current;
        current.gather_stats(qz);
        data.add_stats(current);
        current.raw_data(n, ipct, itrial, &fdc);
      }
      fdc.close();
      fn.str(std::string());
      data.summary_header(&fds);
      fds.close();
    }
  }

#if 0
    if ((itrial > 0) && ((itrial + 1) % 10) == 0) {
      std::cout << "Completed: " << itrial + 1 << " in " << t.sum() << std::endl;
    }
  }
#endif
}
