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
    cd build
    cmake .. && make -j4
    ./cvrp  

<a name="toc"></a>
#### Table of contents: ####
- [Algorithms](#algorithms)
- [Instructions](#instructions)
- [Table of contents](#toc)
- [Documentation](#docs)
- [Overview of Algorithm Implementations](#algorithm_implementations)
- [Notes](#notes)
- [TODOs](#todos)

<a name="docs"></a>
#### Documentation: ####
1. [Documentation](https://vss2sn.github.io/cvrp/) can be found on GitHub pages. 
2. It has been created using Doxygen, and pip3 packages Sphinx (sphinx==1.8.3), Breathe (breathe==4.12.0), Exhale (exhale==0.2.2), Read the Docs Sphinx Theme (sphinx_rtd_theme==0.4.3) and m2r.

<a name="algorithm_implementations"></a>
#### Overview of Algorithm Implementations ####
For a brief overview on the implementations of the algorithms, please refer to the document [here](https://vss2sn.github.io/cvrp/algorithm_implementations.html).

<a name="notes"></a>
#### Notes: ####
1. The metaheuristic parameters need to be set based on the problem size and structure.
2. The problem is setup using the `Problem` class which specifies the number of nodes (centres/dropoff points), maximum demand, number of vehicles, their capacity, the grid range and the type of distribution. The demand for each centre as well as its location is randomly generated.
3. A base class called `Solution` has been created to store the basic elements of the solution in a user friendly format. This includes a number of instances of the `Vehicle` class that store the vehicle id, the route it takes, the total capacity, the number of units still left in the vehicle, and the cost associated with the vehicle's route. A function called `PrintStatus()` can be used to display each of the vehicles attributes.
4. The solution class also contains a pure virtual function called `Solve()`. Each algorithm mentioned above is implemented in its own class by inheriting the Solution class and overriding the `Solve()` method.
5. The documentation for private functions (such as operators in the GASolution class) has been made available to aid understanding.
6. `main.cpp` contains the main function in which the probelm is set up and from which all the algorithms are called.

<a name="todos"></a>
#### TODOs: ####
1. Run all the algorithms on different threads to speed up comparison
2. Refactor
3. Consider adding savings algorithm
4. Refactor problem and solution class to allow multiple algorithms to be applied to same solution sequentially (e.g.:  Simulated annealing followed by local search after a point) allowing for the creation of a hybrid algorithm directly, rather than coding it in separately. Create initial solution iff the current solution is not valid/move to constructor. (Done)
5. Add visualizer using ROS/ROS2.
6. Consider adding maximum distance constraint
7. Consider modifying to allow heterogeneous vehicles.
8. Read in problem from a file.
9. Showcase simple use case of any algorithm in README.md as well as hybrid algorithm case. (Done)
10. Consider adding background of repository.
11. Consider adding plot of best solution cost vs time.
