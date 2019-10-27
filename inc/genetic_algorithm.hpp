/**
* @file genetic_algorithm.hpp
* @author vss2sn
* @brief Contiains the GASolution (Genetic Algorithm Solution) class
*/

#ifndef GA_HPP
#define GA_HPP

#include "utils.hpp"

// Still need to account for case if nodes cannot be put into vehilces due to small number of vehicles in initial solution
class GASolution : public Solution{
public:
  /**
  * @brief Constructor
  * @param nodes Vector of nodes
  * @param vehicles Vector of vehicles
  * @param distanceMatrix Matrix containing distance between each pair of nodes
  * @param n_chromosomes Number of solutions
  * @param generations Number of generations the algorithm should run for
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using GA.
  */
  GASolution(std::vector<Node> nodes,
             std::vector<Vehicle> vehicles,
             std::vector<std::vector<double>> distanceMatrix,
             int n_chromosomes,
             int generations);
  /**
  * @brief Constructor
  * @param p Instance of problem class defining the problem parameters
  * @param n_chromosomes Number of solutions
  * @param generations Number of generations the algorithm should run for
  * @return No return parameter
  * @details Constructor
  */
   GASolution(Problem p,
              int n_chromosomes,
              int generations);

  /**
  * @brief Constructor
  * @param s Instance of Solution class containing a valid solution and problem parameters
  * @param n_chromosomes Number of solutions
  * @param generations Number of generations the algorithm should run for
  * @return No return parameter
  * @details Constructor
  */
   GASolution(Solution s,
              int n_chromosomes,
              int generations);

  /**
  * @brief Function called to solve the given problem using Genetic Algorithm
  * @return void
  * @details Generates random iniitial solutions. Applies selected algorithm. Prints cost of best solution, and its validity.
  */
  void Solve();
private:
  std::vector<std::vector<int>> chromosomes;
  std::vector<std::vector<int>> iterators;
  std::vector<int> chromosome;
  int n_chromosomes;
  int n_nucleotide_pairs;
  int n_vehicles;
  int generations;
  std::vector<double> costs;
  int best;

  /**
  * @brief Generates random solutions
  * @return void
  * @details Generates random solutions for the initial setup of the problem. Atttempts to make sure all the solutions are valid, but no final check to ensure that they are not.
  */
  void GenerateRandomSolutions();

  /**
  * @brief Generates greedy solutions with different start nodes (ie different first nodes after depot considered)
  * @return void
  * @details Generates greedy solution for the initial setup of the problem. Atttempts to make sure all the solutions are valid, and exits the code if the solutions generated are invalid.
  */
  void GenerateGreedySolutions();

  /**
  * @brief Generates a random solution
  * @return void
  * @details Generates a random solution.
  */
  std::vector<int> GenerateRandomSolution();

  /**
  * @brief Generates a random iterator solution
  * @return void
  * @details Generates a random iterator solution containing the points using which the chromosome is to be split into routes for the vehicles
  */
  std::vector<int> GenerateRandomIterSolution();

  /**
  * @brief Heuristic Greedy Crossover (HGreX)
  * @return void
  * @details Two parents are selected using tournament selection and a child is created using the HGreX operator. If the solution is valid, the child is added to the solutions. THe number of solutions is maintained using InsertionBySimilarity()
  */
  void HGreXCrossover();

  /**
  * @brief Swaps two random nucleotide pairs  within a chromosome
  * @return void
  * @details Attempts to swap two random nucleotide pairs  within a chromosome 20 times, if the solution is improved by said swap
  */
  void RandomSwap();

  /**
  * @brief Mutates a chromosome
  * @return void
  * @details Attempts to mutate a random chromosomes 20 times. Each time a chromosome is selected, 2 nucleotide pairs  within the chromosome are selected and the order of all the nucleotide pairs  between those 2 nucleotide pairs  (inclusive) is reversed, if the solution is improved by said operation
  */
  void Mutate();

  /**
  * @brief Deletes the worse chromosome
  * @return void
  * @details Deletes the worse chromosome, ie, the solution with the greatest cost
  */
  void DeleteWorstChromosome();

  /**
  * @brief Removes similar solutions
  * @return void
  * @details Selects solutions that are withing 5% of the cost of another. Removes them with a probability of 0.15. Generates new random solutions to place of the deleted ones tomaintain number of solutions
  */
  void RemoveSimilarSolutions();

  /**
  * @brief Replaces the worst solution with another copy of the best solution
  * @return void
  * @details Replaces the worst solution with another copy of the best solution
  */
  void AddBest();

  /**
  * @brief Mutates a chromosome within an gene
  * @return void
  * @details Attempts to mutate a random chromosomes 20 times. Each time a chromosome is selected, 2 nucleotide pairs  within the chromosome, belonging to the same gene, are selected and the order of all the nucleotide pairs  between those 2 nucleotide pairs  (inclusive) is reversed, if the solution is improved by said operation
  */
  void MutateWhithinGene();

  /**
  * @brief Swaps two random nucleotide pairs  within an gene of a given chromosome
  * @return void
  * @details Attempts to swap two random nucleotide pairs  within an gene of a given chromosome 20 times, ie, selects two nucleotide pairs  within the route of a vehicle and swaps them if the solution is improved by the move
  */
  void SwapWhithinGene();

  /**
  * @brief Split a route between 2 vehicles
  * @return void
  * @details If there exits an unused vehicle (v), splits the route of a random vehicle between said random vehicle and v, if this improves the solution.
  */
  void InsertIterDist();

  /**
  * @brief Decrements a selected point in the iterator vector of a selected chromosome
  * @param i_chromosome Chromosome selected
  * @param j_in Vehicle selected
  * @return bool True if operation successful
  * @details Decrements the value at j_in position of the iterator vector of the chomosome selected, effectively including the last nucleotide pair in the previous gene into the succeeding nucleotide pair. Hence the last visited node from the preceeding vehicle is moved into the route of the current vehicle and is the first node visited by the current vehicle. Operation performed if solution is improved by said operation.
  */
  bool MutateIterLeft(int i_chromosome, int j_in);

  /**
  * @brief Increments a selected point in the iterator vector of a selected chromosome
  * @param i_chromosome Chromosome selected
  * @param j_in Vehicle selected
  * @return bool True if operation successful
  * @details Increments the value at j_in position of the iterator vector of the chomosome selected, effectively including the first nucleotide pair in the current gene into the preceeding nucleotide pair. Hence the first visited node from the current vehicle is moved into the route of the preceeding vehicle and is the last node visited by the preceeding vehicle. Operation performed if solution is improved by said operation.
  */
  bool MutateIterRight(int i_chromosome, int j_in);

  /**
  * @brief Calculates the cost of a given solution
  * @param i The solution whose cost is to be calculated
  * @return double calculated cost
  * @details Calculates the cost of a given solution
  */
  double CalculateCost(int i);

  /**
  * @brief Calculates the cost of all solutions
  * @return void
  * @details Calculates and updates the cost of all solutions
  */
  void CalculateTotalCost();

  /**
  * @brief Tournament selection of a good solution
  * @return int index of solution selected
  * @details Randomly selects 3 solutions and returns index of the best amongst the three
  */
  int TournamentSelection();

  /**
  * @brief Tournament selection of a bad solution
  * @return int index of solution selected
  * @details Randomly selects 3 solutions and returns index of the worst amongst the three
  */
  int TournamentSelectionBad();

  /**
  * @brief Deletes a bad chromosome
  * @return void
  * @details Uses tournament selection to select a bad solution and replace it with a random solution (Currently commented)
  */
  void DeleteBadChromosome();

  /**
  * @brief Removes solution similar to the best soultion and replaces it with a random solution
  * @return void
  * @details Called after a new chromosome is added. If the differenc of the cost of the new solution and an existing solution is with 2% of the cost of the best solution. the existing solution is replaced by the new solution
  */
  void InsertionBySimilarity();

  /**
  * @brief Deletes a random chromosome.
  * @return void
  * @details Deletes a random chromosome that is not the best solution.
  */
  void DeleteRandomChromosome();

  /**
  * @brief Converts the best solution into the usual format
  * @return void
  * @details Splits the chromosome with the lowest cost into the routes indicated by the corresponding iterator vector and places them into the routes of the vehicles
  */
  void GenerateBestSolution();

  /**
  * @brief Checks whether a solution is valid
  * @return bool returns true if the solution is valid (ie the total demand of the nodes on each of the routes does not exceed vehicle capacity)
  * @details Checks whether a solution is valid
  */
  bool checkValidity(int i);

  /**
  * @brief Attempts to make a solution valid
  * @param indes of solution to be made valid
  * @return void
  * @details Iterates over asolution from left right moving iterators left if the demand of a route is greater than the capacity of the vehicle, then iterates over asolution from right to left moving iterators right if the demand of a route is greater than the capacity of the vehicle. If the ordering of the nodes can give a possible solution, this method ensures that it is found. Does not gaurentee a solution if the ordering is impossible (for eg the total demand exceeds the total capacity of all the vehicles)
  */
  void MakeValid(int i);
};

#endif GA_HPP
