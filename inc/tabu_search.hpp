/**
* @file tabu_search.hpp
* @author vss2sn
* @brief Contains the TabuSearchSolution class
*/

#ifndef TS_HPP
#define TS_HPP

#include "utils.hpp"

/**
* @brief Struct to create hash for vector
* @details Contains overloaded operator to create the hash for a vector
*/
struct VectorHash {
  /**
  * @brief Overloaded operator to return hash value of a vector
  * @param vector<int> vector who's hash is to be calculated
  * @return size_t hash of the vector
  * @details Overloaded operator to return hash value of a vector. Used to create an unordered set for vectors.
  */
  size_t operator()(const std::vector<int>& v) const {
      std::hash<int> hasher;
      size_t seed = 0;
      for (int i : v) {
          seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
      }
      return seed;
  }
};

class TabuSearchSolution : public Solution{
public:
  /**
  * @brief Constructor
  * @param nodes Vector of nodes
  * @param vehicles Vector of vehicles
  * @param distanceMatrix Matrix containing distance between each pair of nodes
  * @param n_tabu Size of tabu list
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Tabu Search Algorithm
  */
  TabuSearchSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix, int n_tabu = 50);
  /**
  * @brief Constructor
  * @param p Instance of problem class defining the problem parameters
  * @param n_tabu Size of tabu list
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Tabu Search Algorithm
  */
  TabuSearchSolution(Problem p, int n_tabu = 50);

  /**
  * @brief Constructor
  * @param s Instance of predefined solution containing problem parameters
  * @param n_tabu Size of tabu list
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Tabu Search Algorithm
  */
  TabuSearchSolution(Solution s, int n_tabu = 50);

  /**
  * @brief Function called to solve the given problem using a tabu search algorithm
  * @return void
  * @details Generates random iniitial solutions. Applies selected algorithm. Prints cost of best solution, and its validity.
  */
  void Solve();
private:
  int capacity, n_tabu;
  double best_cost, new_cost;

  std::vector<std::vector<int>> to_check; // invert order of v1, v2 and cur, rep+1
  std::unordered_set<std::vector<int>, VectorHash> tabu_list_set;
  std::queue<std::vector<int>> tabu_list_queue;

  /**
  * @brief Check if set of vector is tabu
  * @param int begin index at which to begin iteration
  * @param int end index at which to end iteration (inclusive)
  * @return bool True if any of the vectors checked are tabu
  * @details Iterates over vector of vectors to_check to check if any of the moves listed in to_check are tabu. to_check is updated for each move considered
  */
  inline bool IsTabu(const int& begin, const int& end);

  /**
  * @brief Aspiration criteria
  * @param double cost_increase Increase in cost due to move
  * @param double cost_reduction Reduction in cost due to move
  * @return bool True if the total cost reduction alls the cost of the solution tobe less than the best cost.
  * @details Ensures that if a move provides a better solution than the best currently available, the move is accepted even if it is tabu
  */
  inline bool Aspiration(double& cost_increase, double& cost_reduction);
};

#endif TS_HPP
