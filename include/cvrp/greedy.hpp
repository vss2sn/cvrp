/**
 * @file greedy.hpp
 * @author vss2sn
 * @brief Contains the GreedySolution class
 */

#ifndef GREEDY_HPP
#define GREEDY_HPP

#include "cvrp/utils.hpp"

class GreedySolution : public Solution {
 public:
  /**
   * @brief Constructor
   * @param nodes Vector of nodes
   * @param vehicles Vector of vehicles
   * @param distanceMatrix Matrix containing distance between each pair of nodes
   * @return No return parameter
   * @details Constructor for initial setup of problem, and solution using
   * greedy algorithm
   */
  GreedySolution(const std::vector<Node>& nodes,
                 const std::vector<Vehicle>& vehicles,
                 const std::vector<std::vector<double>>& distanceMatrix);

  /**
   * @brief Constructor
   * @param p Instance of Problem class defining the problem parameters
   * @return No return parameter
   * @details Constructor for initial setup of problem, and solution using
   * greedy algorithm
   */
  explicit GreedySolution(const Problem& p);

  /**
   * @brief Function called to solve the given problem using a greedy algorithm
   * @return void
   * @details Generates random iniitial solutions. Applies selected algorithm.
   * Prints cost of best solution, and its validity.
   */
  void Solve() override;
};
#endif  // GREEDY_HPP
