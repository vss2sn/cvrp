#include "genetic_algorithm.hpp"

// Still need to account for case if nodes cannot be put into vehilces due to small number of vehicles in initial solution

void GeneticAlgorithmSolution::GenerateRandomSolutions(){
  std::vector<int> temp(n_genes);
  for(int i=0; i<n_genes; ++i){
    temp[i] = i+1;
  }

  for(int i=0;i<n_chromosomes; i++) chromosomes.push_back(temp);
  for(int i=0;i<n_chromosomes; ++i){
    unsigned seed = rand(); //std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(chromosomes[i].begin(), chromosomes[i].end(), std::default_random_engine(seed));
  }
}

double GeneticAlgorithmSolution::CalculateCost(int i){
  int j=0;
  double cost = 0;
  while(j<n_genes){
    // std::cout << "In whle " << j<<" " << nodes[chromosomes[i][j]].demand<< std::endl;
    // std::cout << chromosomes.size() << " " << chromosomes[0].size() << std::endl;
    int load = vehicle_capacity - nodes[chromosomes[i][j]].demand;
    cost += distanceMatrix[0][chromosomes[i][j]];
    while(j+1 < n_genes && load - nodes[chromosomes[i][j+1]].demand >=0){
      // std::cout << "------------" << std::endl;
      load - nodes[chromosomes[i][j+1]].demand;
      // std::cout << "2 " << chromosomes[i][j] << " " <<chromosomes[i][j+1]<<std::endl;
      cost += distanceMatrix[chromosomes[i][j]][chromosomes[i][j+1]];
      j++;
      // std::cout << "3" << std::endl;
    }
    // std::cout << "out" << std::endl;
    cost += distanceMatrix[chromosomes[i][j-1]][0];
    j++;
  }
  // std::cout << "Returning" << std::endl;
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
    std::cout << "Generation: " << generation << std::endl;
    AEXCrossover();
    if(rand()%100==1) Mutate();
    CalculateTotalCost();
    generation++;
    for(int i=0; i< n_chromosomes;i++){
      for(auto& j:chromosomes[i]){
        std::cout << std::setw(3) <<j ;
      }
      std::cout << " | " << costs[i] << std::endl;
    }
  }
  GenerateBestSolution();
  for(auto& v:vehicles) v.PrintRoute();
}

void GeneticAlgorithmSolution::AEXCrossover(){
  int p1 = TournamentSelection();
  int p2 = TournamentSelection();
  std::vector<int> child;
  std::unordered_set<int> reached;
  auto itp1 = &(chromosomes[p1]);
  auto itp2 = &(chromosomes[p2]);
  child.push_back((*itp1)[0]);
  reached.insert(child.back());
  child.push_back((*itp1)[1]);
  reached.insert(child.back());

  while(child.size() < n_genes){
    swap(itp1, itp2);
    auto it = find((*itp1).begin(), (*itp1).end(), child.back());
    // if it = itp1.end() there is something wrong as both chromosomes should contain all the nodes
    if(it!= --itp1->end()
       && reached.find(*(it+1)) == reached.end()){
         reached.insert(*(it+1));
         child.push_back(*(it+1));
       }
    else{
      while(true){
        it++;
        if(it==itp1->end()) it = itp1->begin();
        if(reached.find(*it)==reached.end()){
          child.push_back(*it);
          reached.insert(*it);
          break;
        }
      }
    }
  }
  chromosomes.push_back(child);
  costs.emplace_back(CalculateCost(n_chromosomes));
  DeleteWorstChromosome();
}

int GeneticAlgorithmSolution::TournamentSelection(){
  int i1 = rand()%chromosomes.size();
  int i2 = rand()%chromosomes.size();
  int i3 = rand()%chromosomes.size();
  if(costs[i1] >= costs[i2] && costs[i1] >= costs[i3]) return i1;
  if(costs[i2] >= costs[i1] && costs[i2] >= costs[i3]) return i2;
  if(costs[i3] >= costs[i1] && costs[i3] >= costs[i2]) return i3;
}

void GeneticAlgorithmSolution::InsertionBySimilarity(){}
void GeneticAlgorithmSolution::Mutate(){}

void GeneticAlgorithmSolution::DeleteWorstChromosome(){
  auto it = std::max_element(costs.begin(), costs.end());
  int i = it - costs.begin();
  chromosomes.erase(chromosomes.begin() + i);
  costs.erase(it);
}

void GeneticAlgorithmSolution::GenerateBestSolution(){
  auto it = std::min_element(costs.begin(), costs.end());
  int i = it - costs.begin();
  for(auto j:chromosomes[i]) std::cout << std::setw(3) << j;
  std::cout << std::endl;
  int j=0;
  for(auto& v:vehicles){
    // v.nodes.push_back(0);
    // std::cout << "About to enter of while" << std::endl;
    while(j < n_genes && v.load - nodes[chromosomes[i][j]].demand >= 0){
      // std::cout<< "In while" << std::endl;
      v.cost += distanceMatrix[v.nodes.back()][nodes[chromosomes[i][j]].id];
      v.nodes.push_back(nodes[chromosomes[i][j]].id);
      v.load -= nodes[chromosomes[i][j]].demand;
      j++;
    }
    // std::cout << "Out of while" << std::endl;
    v.cost += distanceMatrix[v.nodes.back()][0];
    v.nodes.push_back(0);
  }
}

int main(){

  std::random_device rd; // obtain a random number from hardware
  std::mt19937 eng(rd()); // seed the generator
  std::uniform_int_distribution<int> ran(0,100); // define the range

  int noc = 50;
	int nov = 10;

  Node depot(50, 50, 0, 0, true);

  std::vector<Node> nodes;
  nodes.push_back(depot);

  for(int i = 1; i <=noc; ++i) nodes.emplace_back(ran(eng), ran(eng), i, ran(eng)/5, false);

  std::vector<std::vector<double>> distanceMatrix;
  std::vector<double> tmp(nodes.size());
  for(int i=0; i<nodes.size(); ++i) distanceMatrix.push_back(tmp);
  for(int i=0; i<nodes.size(); ++i){
    for(int j=i; j < nodes.size(); ++j){
      distanceMatrix[i][j] = sqrt(double(pow((nodes[i].x - nodes[j].x),2)
                                       + pow((nodes[i].y - nodes[j].y),2)));
      distanceMatrix[j][i] = distanceMatrix[i][j];
    }
  }


  std::vector<Vehicle> vehicles;
  int load = 100, capacity = 100;
  for(int i=0; i<nov; ++i){
    vehicles.emplace_back(i, load, capacity);
    vehicles[i].nodes.push_back(0);
  }

  GeneticAlgorithmSolution g(nodes, vehicles, distanceMatrix, 30, 10000 );
  g.Solve();

  return 0;
}