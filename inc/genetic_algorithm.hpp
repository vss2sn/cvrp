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
public:
  GeneticAlgorithmSolution(std::vector<Node> nodes,
                           std::vector<Vehicle> vehicles,
                           std::vector<std::vector<double>> distanceMatrix,
                           int n_chromosomes,
                           int generations)
                           :Solution(nodes, vehicles, distanceMatrix){
                                n_genes = nodes.size();
                                vehicle_capacity = vehicles[0].capacity;
                           };
  void GenerateRandomSolutions();
  double CalculateCost(int i);
  void CalculateTotalCost();
  void Solve();
  void AEXCrossover();
  void TournamentSelection();
  void InsertionBySimilarity();
  void GenerateBestSolution();
}
