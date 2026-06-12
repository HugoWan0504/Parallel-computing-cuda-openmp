#include <math.h>
#include <algorithm>

#include "parallel.h"
using namespace parlay;

/**
 * Inplace block scan DIRECTLY without filter
 */
template <typename T>
T scan(T *A, size_t n) {
  if (n == 0) return 0;

  const size_t BLOCK_SIZE = 10000;
  size_t num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

  T *block_sum = new T[num_blocks];

  // scan-up idea: each block computes local exclusive scan and block total
  parallel_for(0, num_blocks, [&](size_t b) {
    size_t start = b * BLOCK_SIZE;
    size_t end = std::min(start + BLOCK_SIZE, n);

    T total = 0;
    for (size_t i = start; i < end; i++) {
      T tmp = A[i];
      A[i] = total;
      total += tmp;
    }

    block_sum[b] = total;
  });

  // scan the block sums sequentially because num_blocks is much smaller
  T total = 0;
  for (size_t b = 0; b < num_blocks; b++) {
    T tmp = block_sum[b];
    block_sum[b] = total;
    total += tmp;
  }

  // scan-down idea: add each block's offset back to its local scan result
  parallel_for(0, num_blocks, [&](size_t b) {
    size_t start = b * BLOCK_SIZE;
    size_t end = std::min(start + BLOCK_SIZE, n);

    T offset = block_sum[b];
    for (size_t i = start; i < end; i++) {
      A[i] += offset;
    }
  });

  delete[] block_sum;
  return total;
}
