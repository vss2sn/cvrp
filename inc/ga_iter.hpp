#include "utils.hpp"

// Still need to account for case if nodes cannot be put into vehilces due to small number of vehicles in initial solution
class GAIterSolution : public Solution{
private:
  std::vector<std::vector<int>> chromosomes;
  std::vector<std::vector<int>> iterators;
  std::vector<int> chromosome;
  int n_chromosomes;
  int n_genes;
  int n_vehicles;
  int vehicle_capacity;
  int generations;
  std::vector<double> costs;
  int best;
public:
  GAIterSolution(std::vector<Node> nodes,
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
                                n_vehicles = vehicles.size();
                           };
   GAIterSolution(Problem p,
                            int n_chromosomes,
                            int generations)
                            :Solution(p){
                                 this->n_chromosomes = n_chromosomes;
                                 this->generations = generations;
                                 n_genes = nodes.size()-1;
                                 vehicle_capacity = vehicles[0].capacity;
                                 costs = std::vector<double>(n_chromosomes);
                                 n_vehicles = vehicles.size();
                            };
  void GenerateRandomSolutions();
  void GenerateGreedySolutions();
  void NAEXCrossover();
  std::vector<int> GenerateRandomSolution();
  void RandomSwap();
  double CalculateCost(int i);
  void CalculateTotalCost();
  void Solve();
  void AEXCrossover();
  int TournamentSelection();
  int TournamentSelectionBad();
  void DeleteBadChromosome();
  void InsertionBySimilarity();
  void GenerateBestSolution();
  void Mutate();
  void DeleteWorstChromosome();
  void RemoveSimilarSolutions();
  std::vector<int> GenerateRandomIterSolution();
  double NewCalculateCost(int i);
  bool MutateIterLeft(int i_chromosome, int j_in);
  bool MutateIterRight(int i_chromosome, int j_in);
  bool checkValidity(int i);
  void MakeValid(int i);
  void RandomSwapAlele();
  void AddBest();
};
