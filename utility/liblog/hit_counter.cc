#include "hit_counter.h"

namespace cavcom {
  namespace utility {

    HitCounter::HitCounter(const std::string &name)
      : tries_(name + "_tries"), hits_(name + "_hits"), percent_(name + "_pct") {}

    void HitCounter::add(const Counter::value_type &tries, const Counter::value_type &hits) {
      tries_.add(tries);
      hits_.add(hits);
      double total_tries = static_cast<double>(tries_.value());
      double total_hits = static_cast<double>(hits_.value());
      percent_.value((total_tries != 0) ? total_hits/total_tries : 0.0);
    }

    void HitCounter::reset(void) {
      tries_.reset();
      hits_.reset();
      percent_.reset();
    }

  }  // namespace utility
}  // namespace cavcom
