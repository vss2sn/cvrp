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
private:
  std::vector<std::vector<int>> chromosomes;
  std::vector<std::vector<int>> iterators;
  std::vector<int> chromosome;
  int n_chromosomes;
  int n_genes;
  int n_vehicles;
  int generations;
  std::vector<double> costs;
  int best;
public:
  GASolution(std::vector<Node> nodes,
                           std::vector<Vehicle> vehicles,
                           std::vector<std::vector<double>> distanceMatrix,
                           int n_chromosomes,
                           int generations)
                           :Solution(nodes, vehicles, distanceMatrix){
                                this->n_chromosomes = n_chromosomes;
                                this->generations = generations;
                                n_genes = nodes.size()-1;
                                costs = std::vector<double>(n_chromosomes);
                                n_vehicles = vehicles.size();
                           };
   GASolution(Problem p,
                            int n_chromosomes,
                            int generations)
                            :Solution(p){
                                 this->n_chromosomes = n_chromosomes;
                                 this->generations = generations;
                                 n_genes = nodes.size()-1;
                                 costs = std::vector<double>(n_chromosomes);
                                 n_vehicles = vehicles.size();
                            };
  void GenerateRandomSolutions();
  void GenerateGreedySolutions();
  std::vector<int> GenerateRandomSolution();
  std::vector<int> GenerateRandomIterSolution();

  void HGreXCrossover();
  void RandomSwap();
  void Mutate();
  void DeleteWorstChromosome();
  void RemoveSimilarSolutions();
  void RandomSwapAlele();
  void AddBest();
  void MutateWhithinAlele();
  void SwapWhithinAlele();
  void InsertIterDist();
  bool MutateIterLeft(int i_chromosome, int j_in);
  bool MutateIterRight(int i_chromosome, int j_in);

  double CalculateCost(int i);
  void CalculateTotalCost();

  void Solve();

  int TournamentSelection();
  int TournamentSelectionBad();

  void DeleteBadChromosome();
  void InsertionBySimilarity();
  void GenerateBestSolution();

  bool checkValidity(int i);
  void MakeValid(int i);

};

#endif
