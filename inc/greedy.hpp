/**
* @file greedy.hpp
* @author vss2sn
* @brief Contains the GreedySolution class
*/

#ifndef GREEDY_HPP
#define GREEDY_HPP

#include "utils.hpp"

class GreedySolution:public Solution{
public:
  /**
  * @brief Constructor
  * @param nodes Vector of nodes
  * @param vehicles Vector of vehicles
  * @param distanceMatrix Matrix containing distance between each pair of nodes
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using greedy algorithm
  */
  GreedySolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
    :Solution(nodes, vehicles, distanceMatrix){};
  /**
  * @brief Constructor
  * @param p Instance of problem class defining the problem parameters
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using greedy algorithm
  */
  GreedySolution(Problem p)
      :Solution(p.nodes, p.vehicles, p.distanceMatrix){};

  /**
  * @brief Function called to solve the given problem using a greedy algorithm
  * @return void
  * @details Generates random iniitial solutions. Applies selected algorithm. Prints cost of best solution, and its validity.
  */
  void Solve();
};
#endif GREEDY_HPP
