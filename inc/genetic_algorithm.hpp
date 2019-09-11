#include "utils.hpp"

// Still need to account for case if nodes cannot be put into vehilces due to small number of vehicles in initial solution
class GeneticAlgorithmSolution : public Solution{
private:
  std::vector<std::vector<int>> chromosomes;
  std::vector<int> chromosome;
  int n_chromosomes;
  int n_genes;
  int vehicle_capacity;
  int generations;
  std::vector<double> costs;
  int best;
public:
  GeneticAlgorithmSolution(std::vector<Node> nodes,
                           std::vector<Vehicle> vehicles,
                           std::vector<std::vector<double>> distanceMatrix,
                           int n_chromosomes,
                           int generations)
                           :Solution(nodes, vehicles, distanceMatrix){
                                this->n_chromosomes = n_chromosomes;
                                this->generations = generations;
                                n_genes = nodes.size()-1;
                                vehicle_capacity = vehicles[0].capacity;
                                costs = std::vector<double>(n_chromosomes);
                           };
   GeneticAlgorithmSolution(Problem p,
                            int n_chromosomes,
                            int generations)
                            :Solution(p){
                                 this->n_chromosomes = n_chromosomes;
                                 this->generations = generations;
                                 n_genes = nodes.size()-1;
                                 vehicle_capacity = vehicles[0].capacity;
                                 costs = std::vector<double>(n_chromosomes);
                            };
  void GenerateRandomSolutions();
  void GenerateGreedySolutions();
  std::vector<int> GenerateRandomSolution();
  double CalculateCost(int i);
  void CalculateTotalCost();
  void Solve();
  void AEXCrossover();
  int TournamentSelection();
  void InsertionBySimilarity();
  void GenerateBestSolution();
  void Mutate();
  void DeleteWorstChromosome();
  void RemoveSimilarSolutions();
};
