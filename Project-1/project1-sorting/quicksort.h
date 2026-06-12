#include <algorithm>
#include <cstddef>

#include "parallel.h"

using namespace parlay;

template <class T>
static inline T median3(const T &a, const T &b, const T &c) {
    if (a < b) {
        if (b < c) return b;
        return (a < c) ? c : a;
    } else {
        if (a < c) return a;
        return (b < c) ? c : b;
    }
}

template <class T>
static inline T median5(const T &a, const T &b, const T &c,
                        const T &d, const T &e) {
    T m1 = median3(a, b, c);
    T m2 = median3(b, c, d);
    T m3 = median3(c, d, e);
    return median3(m1, m2, m3);
}

template <class T>
static void inplace_sort_medium(T *A, size_t n) {
    const size_t BASE_CASE = 50000;

    if (n <= BASE_CASE) {
        std::sort(A, A + n);
        return;
    }

    T pivot = median3(A[0], A[n / 2], A[n - 1]);

    size_t lt = 0;
    size_t i = 0;
    size_t gt = n - 1;

    // Medium subproblem: in-place 3-way partition avoids tmp/copy overhead.
    while (i <= gt) {
        if (A[i] < pivot) {
            std::swap(A[lt], A[i]);
            lt++;
            i++;
        } else if (pivot < A[i]) {
            std::swap(A[i], A[gt]);
            if (gt == 0) break;
            gt--;
        } else {
            i++;
        }
    }

    size_t left_n = lt;
    size_t right_start = gt + 1;
    size_t right_n = n - right_start;

    par_do(
        [&]() {
            if (left_n > 1) inplace_sort_medium(A, left_n);
        },
        [&]() {
            if (right_n > 1) inplace_sort_medium(A + right_start, right_n);
        }
    );
}

template <class T>
static void quicksort_helper(T *A, T *tmp, size_t n) {
    const size_t BASE_CASE = 50000;
    const size_t HYBRID_CASE = 4000000;
    const size_t BLOCK_SIZE = 8192;

    // Small subproblem: std::sort is faster than parallel overhead.
    if (n <= BASE_CASE) {
        std::sort(A, A + n);
        return;
    }

    // Medium subproblem: use opt1-style in-place quicksort to avoid extra memory passes.
    if (n <= HYBRID_CASE) {
        inplace_sort_medium(A, n);
        return;
    }

    T pivot = median5(A[0], A[n / 4], A[n / 2], A[(3 * n) / 4], A[n - 1]);

    size_t num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

    size_t *less_count = new size_t[num_blocks];
    size_t *equal_count = new size_t[num_blocks];
    size_t *greater_count = new size_t[num_blocks];

    // Parallel count phase.
    parallel_for(0, num_blocks, [&](size_t b) {
        size_t start = b * BLOCK_SIZE;
        size_t end = std::min(start + BLOCK_SIZE, n);

        size_t l = 0;
        size_t e = 0;
        size_t g = 0;

        for (size_t i = start; i < end; i++) {
            if (A[i] < pivot) l++;
            else if (pivot < A[i]) g++;
            else e++;
        }

        less_count[b] = l;
        equal_count[b] = e;
        greater_count[b] = g;
    });

    // Prefix offsets over blocks.
    size_t total_less = 0;
    size_t total_equal = 0;
    size_t total_greater = 0;

    for (size_t b = 0; b < num_blocks; b++) {
        size_t l = less_count[b];
        size_t e = equal_count[b];
        size_t g = greater_count[b];

        less_count[b] = total_less;
        equal_count[b] = total_equal;
        greater_count[b] = total_greater;

        total_less += l;
        total_equal += e;
        total_greater += g;
    }

    // Parallel packing into temporary buffer.
    parallel_for(0, num_blocks, [&](size_t b) {
        size_t start = b * BLOCK_SIZE;
        size_t end = std::min(start + BLOCK_SIZE, n);

        size_t lpos = less_count[b];
        size_t epos = total_less + equal_count[b];
        size_t gpos = total_less + total_equal + greater_count[b];

        for (size_t i = start; i < end; i++) {
            if (A[i] < pivot) tmp[lpos++] = A[i];
            else if (pivot < A[i]) tmp[gpos++] = A[i];
            else tmp[epos++] = A[i];
        }
    });

    // Parallel copy back.
    parallel_for(0, n, [&](size_t i) {
        A[i] = tmp[i];
    }, 4096);

    delete[] less_count;
    delete[] equal_count;
    delete[] greater_count;

    size_t left_n = total_less;
    size_t right_start = total_less + total_equal;
    size_t right_n = total_greater;

    par_do(
        [&]() {
            if (left_n > 1) {
                quicksort_helper(A, tmp, left_n);
            }
        },
        [&]() {
            if (right_n > 1) {
                quicksort_helper(A + right_start, tmp + right_start, right_n);
            }
        }
    );
}

template <class T>
void quicksort(T *A, size_t n) {
    if (n <= 1) return;

    T *tmp = new T[n];
    quicksort_helper(A, tmp, n);
    delete[] tmp;
}
