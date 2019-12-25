#include <iostream>

#include "mycielski.h"
#include "quick_zykov.h"

using namespace cavcom::graph;

int main(int argc, char *argv[]) {
  TikzFormatter formatter(&std::cout);
  Mycielski g(4);
  QuickZykov qz(g, &formatter);
  qz.execute();
  return 0;
}
