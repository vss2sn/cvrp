# Capacitated Vehicle Routing Problem #

### This repository contains algorithms to solve the CVRP (Capacitated Vehicle Routing Problem) in C++. ###

[![Build Status](https://travis-ci.com/vss2sn/cvrp.svg?branch=master)](https://travis-ci.com/vss2sn/cvrp)

<a name="algorithms"></a>
#### Algorithms: ####
1. Greedy Solution
2. Local Search (Run on each vehicle separately; search restricted to the selected vehicle's initial route) (LS)
3. Local Search (LS)
4. Tabu Search (TS)
5. Genetic Algorithm (GA)
6. Simulated Annealing (SA)

<a name="instructions"></a>
#### To build and run: ####
     git clone https://github.com/vss2sn/cvrp.git  
     cd cvrp
     mkdir build  
     cd build  Problem
     cmake .. && make -j4
     ./cvrp  

<a name="toc"></a>
#### Table of contents: ####
- [Algorithms](#algorithms)
- [Instructions](#instructions)
- [Table of contents](#toc)
- [Notes](#notes)
- [Overview of Algorithm Implementations](#algorithm_implementations)
- [TODOs](#todos)

<a name="notes"></a>
#### Notes: ####
1. The metaheuristic parameters need to be set based on the problem size and structure.
2. The problem is setup using the problem class which specifies the number of nodes (centres/dropoff points), maximum demand, number of vehicles, their capacity, the grid range and the type of distribution. The demand for each centre as well as its location is randomly generated.
3. A base class called `Solution` has been created to store the basic elements of the solution in a user friendly format. This includes a number of instances of the `Vehicle` class that store the vehicle id, the route it takes, the total capacity, the number of units still left in the vehicle, and the cost associated with the vehicle's route. A function called `PrintStatus()` can be used to display each of the vehicles attributes.
4. The solution class also contains a pure virtual function called `Solve()`. Each algorithm mentioned above is implemented in its own class by inheriting the Solution class and overriding the `Solve()` method.
5. The documentation for private functions (such as operator in the GASolution class) has been made available to aid understanding.

<a name="algorithm_implementations"></a>
#### Overview of Algorithm Implementations ####
For a brief overview on the implementations of the algorithms, see [here](https://vss2sn.github.io/cvrp/algorithm_implementations.html).

<a name="todos"></a>
#### TODOs: ####
1. Improve Local Search (all vehicle variant) to include vehicles not picked by the initial solution
2. Run all the algorithms on different threads to speed up comparison
3. Refactor
4. Consider adding savings algorithm
