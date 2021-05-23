/**
 * @file main.hpp
 * @author vss2sn
 * @brief Just includes header files.
 */

#include "cvrp/genetic_algorithm.hpp"
#include "cvrp/greedy.hpp"
#include "cvrp/local_search_inter_intra.hpp"
#include "cvrp/local_search_intra.hpp"
#include "cvrp/simulated_annealing.hpp"
#include "cvrp/tabu_search.hpp"

/**
 * @brief Main function
 * @return int 0
 * @details Main function that creates the problem and calls the various solve
 * functions for each of the solution algorithms
 */
int main(int argc, char *argv[]);
