template <class T>
T seq_scan(T *A, size_t n) {
  T total = 0;
  for (size_t i = 0; i < n; i++) {
    T tmp = A[i];
    A[i] = total;
    total += tmp;
  }
  return total;
}
