# Capacitated Vehicle Routing Problem #

### This repository contains implementations of algorithms to solve the CVRP (Capacitated Vehicle Routing Problem) using various methods in C++. ###

<a name="algorithms"></a>
#### Algorithms: ####
1. Greedy Solution
2. Local Search (Run on each vehicle separately; search restricted to the selected vehicle's initial route)
3. Local Search
4. Tabu Search
5. Genetic Algorithm
6. Simulated Annealing

<a name="instructions"></a>
#### To build and run: ####
     git clone https://github.com/vss2sn/cvrp.git  
     cd cvrp
     mkdir build  
     cd build  
     cmake .. && make -j4
     ./cvrp  

<a name="toc"></a>
#### Table of contents: ####
- [Algorithms](#algorithms)
- [Instructions](#instructions)
- [Table of contents](#toc)
- [Notes](#notes)
- [TODOs](#todos)

<a name="notes"></a>
#### Notes: ####
1. This repository is currently in its initial stage where most of the algorithms have been coded in without optimsation.
2. The metaheuristic parameters need to be set based on the problem size and structure.
3. The genetic algorithm is implemented using a vector who's elements point to positions in another vector containing the all nodes in the order in which they will be visited. The first vector mentioned here shows how the second vector is split into the routes of the various vehicles.
4. If there is a node missing in the initial solutions, the algorithm will go into an infinite loop in the HGreXCrossover() function. This can happen if using the GenerateGreedySolutions() function. Check set up to exit should this be the case.
5. While the MakeValid() function ensures an order of nodes visited is valid if it is possible, there is still the possibility that the ordering is not feasible. In this case the final solution might be invalid.

<a name="todos"></a>
#### TODOs: ####
1. Improve Local Search (all vehicle variant) to include vehicles not picked by the initial solution
2. Refactor
3. Code documentation (**Priority**)
4. Consider adding max distance
5. Add references (**Priority**)
6. Set up tests
7. Code coverage integration
8. Run all the algorithms on different threads to speed up comparison
