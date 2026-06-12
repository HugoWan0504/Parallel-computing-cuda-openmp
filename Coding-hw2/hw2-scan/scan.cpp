#include <cstdlib>
#include <iostream>

#include "scan.h"
#include "get_time.h"
#include "scan_seq.h"

using Type = long long;

int main(int argc, char *argv[]) {
  size_t n = 1e9;
  int num_rounds = 3;
  if (argc >= 2) {
    n = atoll(argv[1]);
  }
  if (argc >= 3) {
    num_rounds = atoi(argv[2]);
  }
  Type *seq_scan_A = (Type *)malloc(n * sizeof(Type));
  Type *pal_scan_A = (Type *)malloc(n * sizeof(Type));
  parallel_for(0, n, [&](size_t i) {
    seq_scan_A[i] = i;
    pal_scan_A[i] = i;
  });

  double total_time = 0;
  std::cout << "******* Parallel Results: ******* " << std::endl;
  for (int i = 0; i <= num_rounds; i++) {
    parlay::timer t;
    long long exclusive_sum = scan(seq_scan_A, n);
    t.stop();
    if (i == 0) {
      std::cout << "Exclusive sum (Parallel): " << exclusive_sum << std::endl;
      std::cout << "Warmup round running time: " << t.total_time() << std::endl;
    } else {
      std::cout << "Round " << i << " running time: " << t.total_time()
                << std::endl;
      total_time += t.total_time();
    }
  }

  std::cout << "\n******* Sequential Results: ******* " << std::endl;
  double seq_total_time = 0;
  for (int i = 0; i <= num_rounds; i++) {
    parlay::timer seq_t;
    long long seq_ans = seq_scan(pal_scan_A, n);
    seq_t.stop();

    if (i == 0) {
      std::cout << "Exclusive sum (Sequential): " << seq_ans << std::endl;
      std::cout << "Warmup round running time (Sequential): "
                << seq_t.total_time() << std::endl;
    } else {
      std::cout << "Round " << i
                << " running time (sequential): " << seq_t.total_time()
                << std::endl;
      seq_total_time += seq_t.total_time();
    }
  }
  std::cout << "\n******* Summary: ******* " << std::endl;
  std::cout << "Average sequential running time: "
            << seq_total_time / num_rounds << std::endl;
  std::cout << "Average parallel running time: " << total_time / num_rounds
            << std::endl;
  for (size_t i = 0; i < n; i++) {
    if (seq_scan_A[i] != pal_scan_A[i]) {
      std::cout << "***********************" << std::endl;
      std::cout << "**** Wrong answer! ****" << std::endl;
      std::cout << "***********************" << std::endl;
      break;
    }
  }

  free(seq_scan_A);
  free(pal_scan_A);
  return 0;
}
