#ifndef CAVCOM_GRAPH_LIBGRAPHUTIL_GRAPH_ALGORITHM_H_
#define CAVCOM_GRAPH_LIBGRAPHUTIL_GRAPH_ALGORITHM_H_

#include <chrono>

#include "simple_graph.h"

namespace cavcom {
  namespace graph {

    // The base class for a graph-related algorithm.  It provides runtime markers and step counters.
    class GraphAlgorithm {
     public:
      using Clock = std::chrono::system_clock;
      using Time = Clock::time_point;
      using Duration = Clock::duration;

      // Creates a new graph algorithm instance for an algorithm to be performed on the specified graph.  The
      // start and end time markers are reset to the epoch and all counters are ainitialized to 0.  The graph must
      // remain valid during the lifetime of the algorithm.
      explicit GraphAlgorithm(const SimpleGraph &graph);

      // The original target graph.
      const SimpleGraph &graph() const { return graph_; }

      // Returns the algorithm start/stop time.  An unstarted algorithm has a start time of the epoch.  An
      // unfinished algorithm has an end time of the epoch.
      const Time &start(void) const { return start_; }
      const Time &end(void) const { return end_; }

      // Returns the current/final duration of the algorithm.  If the algorithm has completed then the marked end
      // time is used to calculate the duration.  Otherwise, the current system time is used.
      Duration duration(void) const;

      // Returns true if the algorithm has been start/completed.
      bool started(void) const;
      bool completed(void) const;

      // Returns the number of counted steps.  What constitutes a step is determined by the derived class.  Usually
      // a step is a major algorithm feature.
      ullong steps() const { return steps_; }

      // Returns the number of recursive calls, the current call depth, and the maximum call depth.
      ullong calls() const { return calls_; }
      ullong depth() const { return depth_; }
      ullong maxdepth() const { return maxdepth_; }

      // Executes the algorithm.  All counters are reset to 0.  The start time is set to the current system time
      // and the end time is set to the epoch.  The derived classes run method is then called.  Once the run method
      // returns, the end time is marked.  Returns the return value from the run method.  A true return should
      // indicate that the algorithm was successfully executed.  Other return values are provided by the derived
      // class.  Note that since the target graph is constant, the algorithm can be rerun on the same graph
      // multiple times, if supported by the derived class.
      bool execute();

     protected:
      // Adds a step count.
      void add_step(void);

      // Adds a recursive call count and adjusts the depth trackers.
      void add_call(void);

      // Marks a return from a recursive call.
      void done_call(void);

      // The actual algorithm, provided by the derived algorithm class.
      virtual bool run() { return true; }

     private:
      const SimpleGraph &graph_;

      Time start_;
      Time end_;

      ullong steps_;
      ullong calls_;
      ullong depth_;
      ullong maxdepth_;
    };

  }  // namespace graph
}  // namespace cavcom

#endif  // CAVCOM_GRAPH_LIBGRAPHUTIL_GRAPH_ALGORITHM_H_
