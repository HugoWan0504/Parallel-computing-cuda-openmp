#include "scan.h"
#include <iostream>
#include "scan_seq.h"

using Type = long long;

int main(int argc, char *argv[]) {
  size_t n_1 = 1e9;
  size_t n_2 = 12345678;
  size_t n_3 = 1;
  int flag = 1;

  Type *seq_scan_A_3 = (Type *)malloc(n_3 * sizeof(Type));
  Type *pal_scan_A_3 = (Type *)malloc(n_3 * sizeof(Type));
  parallel_for(0, n_3, [&](size_t i) {
    seq_scan_A_3[i] = i;
    pal_scan_A_3[i] = i;
  });
  long long exclusive_sum = scan(seq_scan_A_3, n_3);
  long long seq_ans = seq_scan(pal_scan_A_3, n_3);
  if (exclusive_sum != seq_ans) { flag = 0; }
  for (size_t i = 0; i < n_3; i ++) {
    if (seq_scan_A_3[i] != pal_scan_A_3[i]) {
        flag = 0;
        std::cout << "Wrong" << std::endl;
    }
  }

  Type *seq_scan_A_1 = (Type *)malloc(n_1 * sizeof(Type));
  Type *pal_scan_A_1 = (Type *)malloc(n_1 * sizeof(Type));
  parallel_for(0, n_1, [&](size_t i) {
    seq_scan_A_1[i] = i;
    pal_scan_A_1[i] = i;
  });
  exclusive_sum = scan(seq_scan_A_1, n_1);
  seq_ans = seq_scan(pal_scan_A_1, n_1);
  for (size_t i = 0; i < n_1; i ++) {
    if (seq_scan_A_1[i] != pal_scan_A_1[i]) {
        flag = 0;
        std::cout << "Wrong" << std::endl;
    }
  }

  Type *seq_scan_A_2 = (Type *)malloc(n_2 * sizeof(Type));
  Type *pal_scan_A_2 = (Type *)malloc(n_2 * sizeof(Type));
  parallel_for(0, n_2, [&](size_t i) {
    seq_scan_A_2[i] = i;
    pal_scan_A_2[i] = i;
  });
  exclusive_sum = scan(seq_scan_A_2, n_2);
  seq_ans = seq_scan(pal_scan_A_2, n_2);
  for (size_t i = 0; i < n_2; i ++) {
    if (seq_scan_A_2[i] != pal_scan_A_2[i]) {
        flag = 0;
        std::cout << "Wrong" << std::endl;
    }
  }
    
  free(seq_scan_A_1);
  free(pal_scan_A_1);
  free(seq_scan_A_2);
  free(pal_scan_A_2);
  free(seq_scan_A_3);
  free(pal_scan_A_3);
  if (flag == 1) {
    std::cout << "Correct" << std::endl;
  } else {
    std::cout << "Wrong" << std::endl;
  }
  return 0;
}
