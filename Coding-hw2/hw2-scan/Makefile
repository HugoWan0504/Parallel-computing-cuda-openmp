ifdef CLANG
CC = clang++
else
CC = g++
endif

CPPFLAGS = -std=c++17 -Wall

ifdef CILKPLUS
CPPFLAGS += -DPARLAY_CILKPLUS -DCILK -fcilkplus
else ifdef SERIAL
CPPFLAGS += -DPARLAY_SEQUENTIAL
else
CPPFLAGS += -pthread
endif

ifdef DEBUG
CPPFLAGS += -Og -mcx16 -DDEBUG
else ifdef PERF
CPPFLAGS += -Og -mcx16 -march=native -g
else ifdef MEMCHECK
CPPFLAGS += -Og -mcx16 -DPARLAY_SEQUENTIAL
else ifdef MXMAC
CPPFLAGS += -O3 -mcpu=apple-m1
else
CPPFLAGS += -O3 -mcx16 -march=native
endif

all: scan

scan: scan.h scan.cpp
	$(CC) $(CPPFLAGS) scan.cpp -o scan

a.out:
	$(CC) $(CPPFLAGS) test.cpp -o test

test: a.out
	bash test.sh

clean:
	rm scan test
