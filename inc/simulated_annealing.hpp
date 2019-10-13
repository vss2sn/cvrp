/**
* @file simulated_annealing.hpp
* @author vss2sn
* @brief Contains the SimulatedAnnealingSolution class
*/

#ifndef SA_HPP
#define SA_HPP

#include "utils.hpp"

class SimulatedAnnealingSolution : public Solution{
public:
  /**
  * @brief Constructor
  * @param nodes Vector of nodes
  * @param vehicles Vector of vehicles
  * @param distanceMatrix Matrix containing distance between each pair of nodes
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Simulated Annealing Algorithm
  */
  SimulatedAnnealingSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
    :Solution(nodes, vehicles, distanceMatrix){} ;

  /**
  * @brief Constructor
  * @param p Instance of problem class defining the problem parameters
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Simulated Annealing Algorithm
  */
  SimulatedAnnealingSolution(Problem p)
    :Solution(p.nodes, p.vehicles, p.distanceMatrix){};

  /**
  * @brief Function called to solve the given problem using a simulated annealing algorithm
  * @return void
  * @details Generates random iniitial solutions. Applies selected algorithm. Prints cost of best solution, and its validity.
  */
  void Solve();
private:
  double temp = 0, cooling_rate = 0.9999, best_cost, current_cost;
  int max_temp = 5000, n_reheates = 20;

  /**
  * @brief Checks whether the move is allowable
  * @param double delta change in cost of solution due to move
  * @return bool True if move accepted
  * @details Allows move if it improves the solution; also allows move based on probability and temperature at the given time.
  */
  inline bool AllowMove(double delta);
};

#endif SA_HPP
