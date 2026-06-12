#include <algorithm>
#include <queue>
#include <set>
#include <climits>

#include "parallel.h"

using namespace parlay;
using namespace std;

void BFS(uint64_t* offsets, uint32_t* edges, uint32_t* dist, size_t n, size_t m,
         uint32_t s) {
  dist[s] = 0;

  double avg_degree = (n == 0) ? 0.0 : (double)m / (double)n;

  // ============================================================
  // RoadUSA-style path:
  // RoadUSA has low average degree and large diameter.
  // Dense bottom-up BFS is too expensive, so never use dense mode.
  // This is kept from Opt9 / Opt10.
  // ============================================================
  if (n > 10000000 && avg_degree < 6.0) {
    uint32_t* frontier = new uint32_t[n];
    uint32_t* next_frontier = new uint32_t[n];

    frontier[0] = s;
    size_t frontier_size = 1;

    const size_t ROAD_PAR_THRESHOLD = 131072;

    while (frontier_size > 0) {
      size_t next_size = 0;

      if (frontier_size < ROAD_PAR_THRESHOLD) {
        // Sequential sparse mode for small RoadUSA frontiers.
        for (size_t idx = 0; idx < frontier_size; idx++) {
          uint32_t u = frontier[idx];
          uint32_t next_dist = dist[u] + 1;

          for (size_t i = offsets[u]; i < offsets[u + 1]; i++) {
            uint32_t v = edges[i];

            if (dist[v] == (uint32_t)INT_MAX) {
              dist[v] = next_dist;
              next_frontier[next_size++] = v;
            }
          }
        }
      } else {
        // Parallel sparse mode only when RoadUSA frontier is large enough.
        parallel_for(0, frontier_size, [&](size_t idx) {
          uint32_t u = frontier[idx];
          uint32_t next_dist = dist[u] + 1;

          for (size_t i = offsets[u]; i < offsets[u + 1]; i++) {
            uint32_t v = edges[i];

            if (__sync_bool_compare_and_swap(&dist[v], (uint32_t)INT_MAX,
                                             next_dist)) {
              size_t pos = __sync_fetch_and_add(&next_size, (size_t)1);
              next_frontier[pos] = v;
            }
          }
        });
      }

      swap(frontier, next_frontier);
      frontier_size = next_size;
    }

    delete[] frontier;
    delete[] next_frontier;
    return;
  }

  // ============================================================
  // Social graph path:
  // Based on Opt10, with minimum LiveJournal-focused retuning.
  //
  // com-orkut:
  //   keep previous high-degree settings.
  //
  // LiveJournal1:
  //   switch to dense mode slightly earlier than Opt10.
  // ============================================================

  uint32_t* frontier = new uint32_t[n];
  uint32_t* next_frontier = new uint32_t[n];

  bool* in_frontier = new bool[n]();
  bool* next_flag = new bool[n]();

  frontier[0] = s;
  size_t frontier_size = 1;

  size_t SEQ_FRONTIER_THRESHOLD;
  size_t DENSE_FRONTIER_THRESHOLD;
  size_t BLOCK_SIZE;

  if (avg_degree > 30.0) {
    // com-orkut-like graph.
    SEQ_FRONTIER_THRESHOLD = 1024;
    DENSE_FRONTIER_THRESHOLD = n / 50;
    BLOCK_SIZE = 4096;
  } else {
    // LiveJournal-like graph.
    SEQ_FRONTIER_THRESHOLD = 131072;
    DENSE_FRONTIER_THRESHOLD = n / 70;
    BLOCK_SIZE = 2048;
  }

  size_t num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

  size_t* block_counts = new size_t[num_blocks];
  size_t* block_offsets = new size_t[num_blocks];

  while (frontier_size > 0) {
    size_t next_size = 0;

    if (frontier_size > DENSE_FRONTIER_THRESHOLD) {
      // Dense / bottom-up mode.
      parallel_for(0, frontier_size, [&](size_t i) {
        in_frontier[frontier[i]] = true;
      });

      // Mark discovered vertices instead of atomically pushing them.
      parallel_for(0, n, [&](size_t v) {
        if (dist[v] == (uint32_t)INT_MAX) {
          for (size_t j = offsets[v]; j < offsets[v + 1]; j++) {
            uint32_t u = edges[j];

            if (in_frontier[u]) {
              dist[v] = dist[u] + 1;
              next_flag[v] = true;
              break;
            }
          }
        }
      });

      // Count discovered vertices per block.
      parallel_for(0, num_blocks, [&](size_t b) {
        size_t start = b * BLOCK_SIZE;
        size_t end = min(start + BLOCK_SIZE, n);

        size_t cnt = 0;
        for (size_t v = start; v < end; v++) {
          if (next_flag[v]) cnt++;
        }

        block_counts[b] = cnt;
      });

      // Prefix sum over block counts.
      size_t sum = 0;
      for (size_t b = 0; b < num_blocks; b++) {
        block_offsets[b] = sum;
        sum += block_counts[b];
      }
      next_size = sum;

      // Fill next frontier in parallel.
      parallel_for(0, num_blocks, [&](size_t b) {
        size_t start = b * BLOCK_SIZE;
        size_t end = min(start + BLOCK_SIZE, n);

        size_t pos = block_offsets[b];
        for (size_t v = start; v < end; v++) {
          if (next_flag[v]) {
            next_frontier[pos++] = v;
            next_flag[v] = false;
          }
        }
      });

      // Clear current frontier bitmap.
      parallel_for(0, frontier_size, [&](size_t i) {
        in_frontier[frontier[i]] = false;
      });

    } else if (frontier_size < SEQ_FRONTIER_THRESHOLD) {
      // Sequential sparse mode.
      for (size_t idx = 0; idx < frontier_size; idx++) {
        uint32_t u = frontier[idx];
        uint32_t next_dist = dist[u] + 1;

        for (size_t i = offsets[u]; i < offsets[u + 1]; i++) {
          uint32_t v = edges[i];

          if (dist[v] == (uint32_t)INT_MAX) {
            dist[v] = next_dist;
            next_frontier[next_size++] = v;
          }
        }
      }

    } else {
      // Parallel sparse mode.
      parallel_for(0, frontier_size, [&](size_t idx) {
        uint32_t u = frontier[idx];
        uint32_t next_dist = dist[u] + 1;

        for (size_t i = offsets[u]; i < offsets[u + 1]; i++) {
          uint32_t v = edges[i];

          if (__sync_bool_compare_and_swap(&dist[v], (uint32_t)INT_MAX,
                                           next_dist)) {
            size_t pos = __sync_fetch_and_add(&next_size, (size_t)1);
            next_frontier[pos] = v;
          }
        }
      });
    }

    swap(frontier, next_frontier);
    frontier_size = next_size;
  }

  delete[] frontier;
  delete[] next_frontier;
  delete[] in_frontier;
  delete[] next_flag;
  delete[] block_counts;
  delete[] block_offsets;
}
