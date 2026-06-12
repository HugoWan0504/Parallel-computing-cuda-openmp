#include "quicksort.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "get_time.h"

inline uint64_t hash64(uint64_t u) {
    uint64_t v = u * 3935559000370003845ul + 2691343689449507681ul;
    v ^= v >> 21;
    v ^= v << 37;
    v ^= v >> 4;
    v *= 4768777513237032717ul;
    v ^= v << 20;
    v ^= v >> 41;
    v ^= v << 5;
    return v;
}

class ZipfGenerator {
  public:
    ZipfGenerator(uint64_t seed, size_t domain, double alpha)
        : rng_(seed), column_dist_(0, domain - 1), tables_(std::make_shared<Tables>()) {
        tables_->prob.resize(domain);
        tables_->alias.resize(domain);

        double total = 0.0;
        for (size_t rank = 1; rank <= domain; rank++) {
            total += 1.0 / std::pow(static_cast<double>(rank), alpha);
        }

        std::vector<double> scaled(domain);
        std::vector<size_t> small;
        std::vector<size_t> large;
        small.reserve(domain);
        large.reserve(domain);

        for (size_t i = 0; i < domain; i++) {
            double weight = 1.0 / std::pow(static_cast<double>(i + 1), alpha);
            scaled[i] = weight * static_cast<double>(domain) / total;
            if (scaled[i] < 1.0) {
                small.push_back(i);
            } else {
                large.push_back(i);
            }
        }

        while (!small.empty() && !large.empty()) {
            size_t less = small.back();
            small.pop_back();
            size_t more = large.back();

            tables_->prob[less] = scaled[less];
            tables_->alias[less] = more;

            scaled[more] += scaled[less] - 1.0;
            if (scaled[more] < 1.0) {
                large.pop_back();
                small.push_back(more);
            }
        }

        for (size_t i : large) {
            tables_->prob[i] = 1.0;
            tables_->alias[i] = i;
        }
        for (size_t i : small) {
            tables_->prob[i] = 1.0;
            tables_->alias[i] = i;
        }
    }

    uint64_t operator()() {
        size_t column = column_dist_(rng_);
        if (prob_dist_(rng_) < tables_->prob[column]) {
            return static_cast<uint64_t>(column);
        }
        return static_cast<uint64_t>(tables_->alias[column]);
    }

  private:
    struct Tables {
        std::vector<double> prob;
        std::vector<size_t> alias;
    };

    std::mt19937_64 rng_;
    std::uniform_int_distribution<size_t> column_dist_;
    std::uniform_real_distribution<double> prob_dist_{0.0, 1.0};
    std::shared_ptr<Tables> tables_;
};

template <class T>
void mixup(T* A, size_t n, int test_case, uint64_t random_seed) {
    switch (test_case) {
        case 1: {
            std::mt19937_64 rng(random_seed);
            std::uniform_int_distribution<uint64_t> dist(0, 1000000000ull);
            for (size_t i = 0; i < n; i++) {
                A[i] = dist(rng);
            }
            break;
        }
        case 2: {
            ZipfGenerator gen(random_seed + 17, 1000000ull, 1.20);
            for (size_t i = 0; i < n; i++) {
                A[i] = gen();
            }
            break;
        }
        case 3: {
            std::mt19937_64 rng(random_seed + 31);
            std::uniform_int_distribution<uint64_t> dist(0, 100ull);
            for (size_t i = 0; i < n; i++) {
                A[i] = dist(rng);
            }
            break;
        }
        case 4: {
            ZipfGenerator gen(random_seed + 47, 101ull, 1.50);
            for (size_t i = 0; i < n; i++) {
                A[i] = gen();
            }
            break;
        }
        case 5: {
            std::mt19937_64 rng(random_seed + 61);
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            for (size_t i = 0; i < n; i++) {
                double u = dist(rng);
                double v = dist(rng);
                double value;
                if (u < 0.60) {
                    value = 0.10 + 0.10 * v;
                } else if (u < 0.85) {
                    value = 0.55 + 0.02 * v;
                } else {
                    value = v;
                }
                A[i] = static_cast<T>(value);
            }
            break;
        }
        default:
            parallel_for(0, n, [&](size_t j) { A[j] = hash64((j + 1) * random_seed); });
            break;
    }
}

template <class T>
int run_benchmark(int test_case, size_t n, int num_rounds, uint64_t random_seed) {
    T* A = static_cast<T*>(std::malloc(n * sizeof(T)));
    T* B = static_cast<T*>(std::malloc(n * sizeof(T)));
    T* C = static_cast<T*>(std::malloc(n * sizeof(T)));
    mixup(C, n, test_case, random_seed);
    parallel_for(0, n, [&](size_t j) { B[j] = C[j]; });
    std::sort(B, B + n);

    parlay::timer t; double total_time = 0, tt = 0;
    for (int i = 0; i <= num_rounds; i++) {
        parallel_for(0, n, [&](size_t j) { A[j] = C[j]; });
        t.start();
        quicksort(A, n);
        tt = t.stop();
        parallel_for(0, n, [&](size_t j) {
            if (A[j] != B[j]) {
                std::cout << "The output is not sorted\n";
                std::exit(0);
            }
        });
        if (i == 0) {
            std::cout << "Warmup round running time: " << tt << std::endl;
        } else {
            std::cout << "Round " << i << " running time: " << tt << std::endl;
            total_time += tt;
        }
    }
    std::cout << "Average running time: " << total_time / num_rounds << std::endl;

    std::free(A); std::free(B); std::free(C);
    return 0;
}

int main(int argc, char* argv[]) {
    int test_case = 1;
    size_t n = 1e8;
    int num_rounds = 3;
    uint64_t random_seed = 1;
    if (argc >= 2) { test_case = atoll(argv[1]); }
    if (argc >= 3) { n = atoll(argv[2]); }
    if (argc >= 4) { num_rounds = atoi(argv[3]); }
    if (argc >= 5) { random_seed = std::strtoull(argv[4], nullptr, 10); }

    if (test_case == 5) {
        return run_benchmark<double>(test_case, n, num_rounds, random_seed);
    }
    else return run_benchmark<uint64_t>(test_case, n, num_rounds, random_seed);
}
