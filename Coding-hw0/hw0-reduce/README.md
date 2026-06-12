[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/JWaDJEJ0)
# homework0-reduce  

## Access github on course server

If you need to access git on the course server, you may need to set the SSH key in Github. You can follow the steps below:

- Log into the server
- Then, generate the SSH key by
```
ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
```
- Then, find your public key in file ``~/.ssh/id_rsa.pub``
- Finally, copy the content of this file. Go to GitHub => Your profile => SSH and GPG keys. Click "New SSH key". Paste the public key into the field "Key", and give it a name you like. 
- You are now able to access the git repository on the course server!
  
## Compiling the code  
A Makefile is given in the directory, simply use ``make`` to compile the code. If you're compiling the code on a M-series Mac, add the ``MXMAC=1`` option:  
```bash
make MXMAC=1  
```
## Running the code  
You can run the code with:  
```bash
./reduce [num_elements] [num_rounds]  
```
If not specified, the default values are ``num_elements=1000000000`` and ``num_rounds=3``.  

## Changing the number of threads  
In your **command line**, set the environment variable ``PARLAY_NUM_THREADS`` using ``export``. For example, set the number of threads to 4:  
```bash
export PARLAY_NUM_THREADS=4  
```

## Adding granularity control  
Edit the the following function in the file``reduce.h``:  
```C++
template <class T>
T reduce(T *A, size_t n) {
  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return A[0];
  } else {
    T v1, v2;
    auto f1 = [&]() { v1 = reduce(A, n / 2); };
    auto f2 = [&]() { v2 = reduce(A + n / 2, n - n / 2); };
    par_do(f1, f2);
    return v1 + v2;
  }
}
```
when $n$ is small enough, add the sum iteratively in sequential instead of dividing the tasks and computing the sum in parallel.  

