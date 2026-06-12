#include "integral.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "get_time.h"

using Type = long long;

int main(int argc, char* argv[]) {
  size_t n = 1e9;
  double low = 1;
  double high = 2;
  int num_rounds = 3;
  if (argc >= 2) {
    n = atoll(argv[1]);
  }
  if (argc >= 3) {
    low = atof(argv[2]);
  }
  if (argc >= 4) {
    high = atof(argv[3]);
  }
  if (argc >= 5) {
    num_rounds = atoi(argv[4]);
  }

  double total_time = 0;
  for (int i = 0; i <= num_rounds; i++) {
    parlay::timer t;
    auto f = [](double x) { return sin(x) * log(x); };
    double ans = integral(f, n, low, high);
    t.stop();

    if (i == 0) {
      std::cout << "Total sum: " << std::setprecision(13) << ans << std::endl;
      std::cout << "Warmup round running time: " << t.total_time() << std::endl;
    } else {
      std::cout << "Round " << i << " running time: " << t.total_time()
                << std::endl;
      total_time += t.total_time();
    }
  }
  std::cout << "Average running time: " << total_time / num_rounds << std::endl;

  return 0;
}
