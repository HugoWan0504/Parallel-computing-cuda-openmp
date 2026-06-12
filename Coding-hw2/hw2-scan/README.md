[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/GwGcrIcM)
# homework0-reduce  

## Compiling the code  
A Makefile is given in the directory, simply use ``make`` to compile the code. If you're compiling the code on a M-series Mac, add the ``MXMAC=1`` option:  
```bash
make MXMAC=1  
```
## Running the code  
You can run the code with:  
```bash
./scan [num_elements] [num_rounds]  
```
If not specified, the default values are ``num_elements=1000000000`` and ``num_rounds=3``.  

## Changing the number of threads  
In your **command line**, set the environment variable ``PARLAY_NUM_THREADS`` using ``export``. For example, set the number of threads to 4:  
```bash
export PARLAY_NUM_THREADS=4  
```

