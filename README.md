# Capacitated Vehicle Routing Problem #

### This repository contains implementations of algorithms to solve the CVRP (Capacitated Vehicle Routing Problem) using various methods in C++. ###

### Algorithms: ###
1. Greedy Solution
2. Local Search (Within each vehicle chosen by initial greedy solution only)
3. Local Search (Within all vehicles chosen by initial greedy solution)
4. Tabu Search (Within all vehicles chosen by initial greedy solution)
5. Genetic Algorithm (Includes manipulation of iterators that point to positions in the gene from where the next vehicle is to begin)
6. Simulated Annealing

#### To build and run: ####
     git clone https://github.com/vss2sn/cvrp.git  
     cd cvrp
     mkdir build  
     cd build  
     cmake .. && make -j
     ./cvrp  

### Notes: ###
1. This repository is currently in its initial stage where most of the algorithms have been coded in without optimsation.
2. The metaheuristic parameters need to be set based on the problem size and structure.

### TODOs: ###
1. Improve Local Search (all vehicle variant) to include vehicles not picked by the initial solution
2. Refactor
3. Code documentation (**Priority**)
4. Consider adding max distance
5. Add references (**Priority**)
6. Set up tests
7. Code coverage integration
8. Run all the algorithms on different threads to speed up comparison.
