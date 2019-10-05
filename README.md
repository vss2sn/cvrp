# Capacitated Vehicle Routing Problem #

### This repository contains implementations of algorithms to solve the CVRP (Capacitated Vehicle Routing Problem) using various methods in C++. ###

### Algorithms: ###
1. Greedy Solution
2. Local Search (Within each vehicle chosen by initial greedy solution only)
3. Local Search (Within all vehicles chosen by initial greedy solution)
4. Tabu Search (Within all vehicles chosen by initial greedy solution)
5. Genetic Algorithm (Includes manipulation of iterators chat point to positions in the gene where the next vehicle is to begin)

#### To build and run: ####
     git clone https://github.com/vss2sn/cvrp.git  
     cd cvrp
     mkdir build  
     cd build  
     cmake .. && make -j
     ./cvrp  

### Notes: ###
1. This repository is currently in its initial stage where most of the algorithms have been coded in without optimsation.

### TODOs: ###
1. Next algorithm: Simulated Annealing
2. Improve Local Search (all vehicle variant) to include vehicles not picked by the initial solution
3. Refactor
4. Code documentation
5. Consider adding max distance
