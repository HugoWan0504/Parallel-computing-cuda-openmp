[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/IZ--Lfpi)
# homework1-integral  

## Compiling the code  
A Makefile is given in the directory, simply use ``make`` to compile the code. If you're compiling the code on a M-series Mac, add the ``MXMAC=1`` option:  
```bash
make MXMAC=1  
```
## Running the code  
You can run the code with:  
```bash
./integral [num_elements] [low] [high] [num_rounds]  
```
If not specified, the default values are ``num_elements=1000000000``, ``low=1``, ``high=2``, and ``num_rounds=3``.  

## Applying parallelism
Parallelize a for-loop:  
```C++
// sequential version  
for(int i = 0; i < n; i++) {
  f(i);
}
// parallel version  
parallel_for(0, n, [&](size_t i) {
  f(i);
});
```

Parallelize two statements:  
```C++
// sequential version
a += b;
c += d;
// parallel version
auto f1 = [&]() { a += b; };
auto f2 = [&]() { c += d; };
par_do(f1, f2);
```
