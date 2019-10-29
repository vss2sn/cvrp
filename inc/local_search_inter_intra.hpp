/**
* @file local_search_inter_intra.hpp
* @author vss2sn
* @brief Contains the LocalSearchInterIntraSolution class (Local search extends to all vehicles)
*/

#ifndef LSII_HPP
#define LSII_HPP

#include "utils.hpp"

class LocalSearchInterIntraSolution : public Solution{
public:
  /**
  * @brief Constructor
  * @param nodes Vector of nodes
  * @param vehicles Vector of vehicles
  * @param distanceMatrix Matrix containing distance between each pair of nodes
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Local Search within all vehicles
  */
  LocalSearchInterIntraSolution(std::vector<Node> nodes,
                                std::vector<Vehicle> vehicles,
                                std::vector<std::vector<double>> distanceMatrix);

  /**
  * @brief Constructor
  * @param p Instance of Problem class defining the problem parameters
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Local Search within all vehicles
  */
  LocalSearchInterIntraSolution(Problem p);

  /**
  * @brief Constructor
  * @param s Instance of Solution class containing a valid solution and problem parameters
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Local Search within all vehicles
  */
  LocalSearchInterIntraSolution(Solution s);
  /**
  * @brief Function called to solve the given problem using a local search algorithm
  * @return void
  * @details Generates random iniitial solutions. Applies selected algorithm. Prints cost of best solution, and its validity.
  */
  void Solve();
};

#endif // LSII_HPP
