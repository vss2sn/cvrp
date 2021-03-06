/**
 * @file tabu_search.hpp
 * @author vss2sn
 * @brief Contains the TabuSearchSolution class
 */

#ifndef TS_HPP
#define TS_HPP

#include <queue>
#include <unordered_set>
#include <utility>

#include "cvrp/utils.hpp"

/**
 * @brief Struct to create hash for pair
 */
struct PairHash {
  /**
   * @brief Overloaded operator to return hash value of a pait
   * @param p pair who's hash is to be calculated
   * @return size_t hash of the pair
   */
  template <typename T1, typename T2>
  size_t operator()(const std::pair<T1, T2>& p) const {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

class TabuSearchSolution : public Solution {
 public:
  /**
   * @brief Constructor
   * @param nodes Vector of nodes
   * @param vehicles Vector of vehicles
   * @param distanceMatrix Matrix containing distance between each pair of nodes
   * @param n_tabu Size of tabu list
   * @param max_it Number of iterations of search
   * @return No return parameter
   * @details Constructor for initial setup of problem, and solution using Tabu
   * Search Algorithm
   */
  TabuSearchSolution(const std::vector<Node>& nodes,
                     const std::vector<Vehicle>& vehicles,
                     const std::vector<std::vector<double>>& distanceMatrix,
                     const int n_tabu = 50, const int max_it = 500);
  /**
   * @brief Constructor
   * @param p Instance of Problem class defining the problem parameters
   * @param n_tabu Size of tabu list
   * @param max_it Number of iterations of search
   * @return No return parameter
   * @details Constructor for initial setup of problem, and solution using Tabu
   * Search Algorithm
   */
  explicit TabuSearchSolution(const Problem& p, const int n_tabu = 50,
                              const int max_it = 500);

  /**
   * @brief Constructor
   * @param s Instance of Solution class containing a valid solution and problem
   * parameters
   * @param n_tabu Size of tabu list
   * @param max_it Number of iterations of search
   * @return No return parameter
   * @details Constructor for initial setup of problem, and solution using Tabu
   * Search Algorithm
   */
  explicit TabuSearchSolution(const Solution& s, const int n_tabu = 50,
                              const int max_it = 500);

  /**
   * @brief Function called to solve the given problem using a tabu search
   * algorithm
   * @return void
   * @details Generates random iniitial solutions. Applies selected algorithm.
   * Prints cost of best solution, and its validity.
   */
  void Solve() override;

 private:
  int n_tabu_;
  const int max_it_;
  double best_cost_ = std::numeric_limits<double>::max();
  double new_cost_ = std::numeric_limits<double>::max();

  // invert order of v1, v2 and cur, rep+1
  std::unordered_set<std::pair<int, int>, PairHash> tabu_list_set_;
  std::queue<std::pair<int, int>> tabu_list_queue_;

  /**
   * @brief Check if set of vector is tabu
   * @param p the pair that must be checked in the tabu list
   * @return bool True if any of the vectors checked are tabu
   * @details Iterates over vector of vectors to_check to check if any of the
   * moves listed in to_check are tabu. to_check is updated for each move
   * considered
   */
  inline bool IsTabu(const std::pair<int, int>& p) const;

  /**
   * @brief Aspiration criteria
   * @param cost_increase Increase in cost due to move
   * @param cost_reduction Reduction in cost due to move
   * @return bool True if the total cost reduction alls the cost of the solution
   * tobe less than the best cost.
   * @details Ensures that if a move provides a better solution than the best
   * currently available, the move is accepted even if it is tabu
   */
  inline bool Aspiration(double cost_increase, double cost_reduction) const;
};

#endif  // TS_HPP
