#include "utils.hpp"

// Still need to account for case if nodes cannot be put into vehilces due to small number of vehicles in initial solution
class GeneticAlgorithmSolution : public Solution{
public:
  std::vector<std::vector<int>> chromosomes;
  std::vector<int> chromosome;
  int n_chromosomes;
  int n_genes;
  int vehicle_capacity;
  int generations;
  std::vector<double> costs;
  // each dropoff point is a gene;
  void GenerateRandomSolutions(){
    vector<int> temp;
    for(int i=0; i<n_genes; ++i){
      temp[i] = i+1;
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    chromosomes = std::vector<std::vector<int>>(temp, n_chromosomes);
    for(int i=0;i<n_chromosomes; ++i){
      shuffle(chromosomes[i].begin(), chromosomes[i].end(), std::default_random_engine(seed));
    }
  }

  double CalculateCost(int i){
    int j=0 int v_id;
    double cost = 0;
    while(j<n_genes){
      int load = vehicle_capacity - chromosomes[i][j].demand;
      if(load < 0){
        std::cout << "Demand at a single dropoff point > capacity" << std::endl;
        j++;
        continue;
      }
      cost += distanceMatrix[0][chromosomes[i][j]];
      while(load - nodes[chromosomes[i][j+1]].demand >=0){
        load - nodes[chromosomes[i][j+1]].demand;
        cost += distanceMatrix[chromosomes[i][j]][chromosomes[i][j+1]];
        j++;
      }
      cost += distanceMatrix[chromosomes[i][j]][0];
    }
    return cost;
  }

  void CalculateTotalCost(){
    for(int i=0;i<n_chromosomes;i++) costs[i] = CalculateCost(i);
  }

  void Solve(){
    GenerateRandomSolutions();
    CalculateTotalCost();
    int generation = 0;
    while(generation < generations){
      AEXCrossover();
      if(rand()%100==1) Mutate();
      CalculateTotalCost();
    }
    GenerateBestSolution();
  }

  void AEXCrossover(){};
  void TournamentSelection(){}
  void InsertionBySimilarity(){};

  void GenerateBestSolution(){
    auto it = std::min_element(costs.begin(), costs.end());
    int i = it - costs.begin();
    int j=0;
    for(auto& v:vehicles){
      v.nodes.push_back(0);
      while(v.load - nodes[chromosomes[i][j]].demand >= 0){
        v.cost += distanceMatrix[v.nodes.back()][nodes[chromosomes[i][j]].id];
        v.nodes.push_back(nodes[chromosomes[i][j]].id);
        v.load -= nodes[chromosomes[i][j]].demand;
      }
      v.cost += distanceMatrix[v.nodes.back()[0];
      v.nodes.push_back(0);
    }
  }
}
