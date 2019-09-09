#include "geneticalgorithm.hpp"

// Still need to account for case if nodes cannot be put into vehilces due to small number of vehicles in initial solution

void GeneticAlgorithmSolution::GenerateRandomSolutions(){
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

double GeneticAlgorithmSolution::CalculateCost(int i){
  int j=0 int v_id;
  double cost = 0;
  while(j<n_genes){
    int load = vehicle_capacity - chromosomes[i][j].demand;
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

void GeneticAlgorithmSolution::CalculateTotalCost(){
  for(int i=0;i<n_chromosomes;i++) costs[i] = CalculateCost(i);
}

void GeneticAlgorithmSolution::Solve(){
  GenerateRandomSolutions();
  CalculateTotalCost();
  int generation = 0;
  while(generation < generations){
    AEXCrossover();
    if(rand()%100==1) Mutate();
    CalculateTotalCost();
  }
  GenerateBestSolution();
  for(auto& v:vehicles) v.PrintRoute();
}

void GeneticAlgorithmSolution::AEXCrossover(){}

int GeneticAlgorithmSolution::TournamentSelection(){
  int i1 = rand()%chromosomes.size();
  int i2 = rand()%chromosomes.size();
  int i3 = rand()%chromosomes.size();
  if(costs[i1] >= costs[i2] && costs[i1] >= costs[i3]) return i1;
  if(costs[i2] >= costs[i1] && costs[i2] >= costs[i3]) return i2;
  if(costs[i3] >= costs[i1] && costs[i3] >= costs[i2]) return i3;
}

void GeneticAlgorithmSolution::InsertionBySimilarity(){}

void GeneticAlgorithmSolution::GenerateBestSolution(){
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
