#include "parallel.h"

using namespace parlay;

template <class Func>
double integral(const Func& f, size_t n, double low, double high) {
    const size_t BLOCK_SIZE = 500000;
    size_t num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

    double* partial = new double[num_blocks];
    double dx = (high - low) / n;

    parallel_for(0, num_blocks, [&](size_t b) {
        size_t start = b * BLOCK_SIZE;
        size_t end = std::min(start + BLOCK_SIZE, n);

        double local_sum = 0.0;
        for (size_t i = start; i < end; i++) {
            double x = low + (high - low) * i / n;
            local_sum += f(x) * dx;
        }

        partial[b] = local_sum;
    });

    double ans = 0.0;
    for (size_t b = 0; b < num_blocks; b++) {
        ans += partial[b];
    }

    delete[] partial;
    return ans;
}
