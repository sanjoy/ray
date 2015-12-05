#include "support.hpp"

#include <cstdio>
#include <cstdlib>

using namespace ray;
using namespace std;

void ray::unreachable(const char *msg) {
  fprintf(stderr, "unreachable executed: %s", msg);
  fflush(stderr);
  abort();
}
