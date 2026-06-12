# Parallel Computing CUDA OpenMP

A compact portfolio of parallel computing implementations and performance experiments using C++, OpenMP-style CPU parallelism, and CUDA/GPU computing concepts.

This repository focuses on parallel algorithm design, scalability, synchronization, memory behavior, and runtime optimization. It includes foundational parallel primitives, larger algorithmic workloads, written analysis, and presentation materials related to high-performance computing.

## Overview

This repository demonstrates experience with:

- C++ parallel programming
- CPU multithreading
- OpenMP-style parallel execution
- CUDA/GPU computing concepts
- Parallel algorithm design
- Synchronization and correctness reasoning
- Runtime benchmarking
- Scalability and performance analysis

## Coding Implementations

The coding assignments cover core parallel computing patterns and progressively move from simpler primitives to more complex workloads.

### Parallel Reduction

Implements parallel reduction for summing large arrays.

This work focuses on divide-and-conquer parallelism, task granularity, and thread scaling. It highlights the tradeoff between exposing enough parallel work and avoiding excessive parallel overhead.

Key topics:

- Parallel reduction
- Recursive decomposition
- Granularity control
- Runtime comparison
- Thread scaling

### Parallel Numerical Integration

Implements numerical integration using parallel work decomposition.

This work focuses on dividing independent computation across parallel workers and combining partial results efficiently.

Key topics:

- Parallel loops
- Independent workload partitioning
- Reduction-style accumulation
- Correctness checking
- Performance comparison

### Parallel Prefix Scan

Implements parallel prefix scan, also known as prefix sum.

Prefix scan is a core primitive used in many parallel algorithms, including filtering, sorting, graph traversal, and stream compaction.

Key topics:

- Prefix sum
- Parallel scan
- Sequential baseline comparison
- Correctness testing
- Parallel primitive design

## Larger Parallel Workloads

### Parallel Quicksort

Implements parallel quicksort with multiple optimization versions.

This project focuses on recursive task parallelism, divide-and-conquer sorting, and reducing parallel overhead through implementation improvements.

Key topics:

- Parallel sorting
- Recursive divide-and-conquer
- Baseline vs optimized versions
- Task overhead reduction
- Runtime benchmarking

### Parallel Breadth-First Search

Implements parallel breadth-first search for graph traversal workloads.

This project focuses on irregular parallel workloads, where performance is affected by uneven work distribution, synchronization, and memory access patterns.

Key topics:

- Parallel BFS
- Frontier-based graph traversal
- Parallel neighbor exploration
- Load balancing
- Irregular memory access
- Scalability analysis

## Written Analysis and Presentation Materials

The written analysis materials document concepts related to parallel algorithms, scheduling, synchronization, correctness, and performance reasoning.

The presentation materials focus on concurrent data structures, especially linearizable queues, including correctness under interleavings and lock-free reasoning.

## Technical Skills Demonstrated

- C++
- Parallel algorithms
- OpenMP-style parallel programming
- CUDA/GPU computing concepts
- Divide-and-conquer programming
- Reduction and scan primitives
- Parallel sorting
- Parallel graph traversal
- Runtime benchmarking
- Thread scaling analysis
- Granularity control
- Scheduler-aware programming
- Synchronization and correctness reasoning
- Performance report writing

## Performance Focus

The main performance questions explored in this repository include:

- How much speedup can parallel execution provide?
- When does parallel overhead outweigh the benefit of additional tasks?
- How does task granularity affect runtime?
- How do synchronization and memory behavior affect scalability?
- Why do regular workloads scale differently from irregular graph workloads?
- Which implementation changes lead to measurable runtime improvements?

## Summary

This repository is a compact parallel computing portfolio covering foundational primitives and larger workloads. It starts with reduction, numerical integration, and prefix scan, then extends to parallel quicksort and graph BFS.

The overall focus is on correctness, scalability, benchmarking, and practical optimization in parallel computing systems.