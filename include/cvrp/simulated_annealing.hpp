/**
 * @file simulated_annealing.hpp
 * @author vss2sn
 * @brief Contains the SimulatedAnnealingSolution class
 */

#ifndef SA_HPP
#define SA_HPP

#include "cvrp/utils.hpp"

class SimulatedAnnealingSolution : public Solution {
 public:
  /**
   * @brief Constructor
   * @param nodes Vector of nodes
   * @param vehicles Vector of vehicles
   * @param distanceMatrix Matrix containing distance between each pair of nodes
   * @param stag_limit Number of iterations without an improvement to the best
   * solution
   * @param init_temp Initial temperature
   * @param cooling_rate tempertaure multiplies with this variable to get the
   * new temperature at every iteration
   * @param n_reheats number of reheats
   * @return No return parameter
   * @details Constructor for initial setup of problem, and solution using
   * Simulated Annealing Algorithm
   */
  SimulatedAnnealingSolution(
      const std::vector<Node>& nodes, const std::vector<Vehicle>& vehicles,
      const std::vector<std::vector<double>>& distanceMatrix,
      const int stag_limit = 500000, const double init_temp = 5000,
      const double cooling_rate = 0.9999, const int n_reheats = 20);

  /**
   * @brief Constructor
   * @param p Instance of Problem class defining the problem parameters
   * @param stag_limit Number of iterations without an improvement to the best
   * solution
   * @param init_temp Initial temperature
   * @param cooling_rate tempertaure multipluies with this variable to get new
   * temperature at every iteration
   * @param n_reheats number of reheats
   * @return No return parameter
   * @details Constructor for initial setup of problem, and solution using
   * Simulated Annealing Algorithm
   */
  explicit SimulatedAnnealingSolution(const Problem& p,
                                      const int stag_limit = 500000,
                                      const double init_temp = 5000,
                                      const double cooling_rate = 0.9999,
                                      const int n_reheats = 20);

  /**
   * @brief Constructor
   * @param s Instance of Solution class containing a valid solution and problem
   * parameters
   * @param stag_limit Number of iterations without an improvement to the best
   * solution
   * @param init_temp Initial temperature
   * @param cooling_rate tempertaure multipluies with this variable to get new
   * temperature at every iteration
   * @param n_reheats number of reheats
   * @return No return parameter
   * @details Constructor for initial setup of problem, and solution using
   * Simulated Annealing Algorithm
   */
  explicit SimulatedAnnealingSolution(const Solution& s,
                                      int stag_limit = 500000,
                                      double init_temp = 5000,
                                      double cooling_rate = 0.9999,
                                      const int n_reheats = 20);

  /**
   * @brief Function called to solve the given problem using a simulated
   * annealing algorithm
   * @return void
   * @details Generates random iniitial solutions. Applies selected algorithm.
   * Prints cost of best solution, and its validity.
   */
  void Solve() override;

 private:
  const int stag_limit_;
  const double init_temp_;
  const double cooling_rate_;
  const int n_reheats_;
  /**
   * @brief Checks whether the move is allowable
   * @param delta change in cost of solution due to move
   * @param temp current temperature
   * @return bool True if move accepted
   * @details Allows move if it improves the solution; also allows move based on
   * probability and temperature at the given time.
   */
  inline static bool AllowMove(const double delta, const double temp);
};

#endif  // SA_HPP
