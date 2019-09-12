#include "genetic_algorithm.hpp"

// Still need to account for case if nodes cannot be put into vehilces due to small number of vehicles in initial solution

std::vector<int> GANaiveSolution::GenerateRandomSolution(){
  std::vector<int> temp(n_genes);
  for(int i=0; i<n_genes; ++i){
    temp[i] = i+1;
  }
  unsigned seed = rand(); //std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(temp.begin(), temp.end(), std::default_random_engine(seed));
  return temp;
}

void GANaiveSolution::GenerateRandomSolutions(){
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

void GANaiveSolution::GenerateGreedySolutions(){
  std::vector<int> gs;
  auto vehicles2 = vehicles;
  for(auto& v:vehicles2){
    while(true){
      Node closest_node = find_closest(v, distanceMatrix, nodes);
      if(closest_node.id!=-1 && v.load - closest_node.demand >=0){
        v.load -= closest_node.demand;
        v.cost += distanceMatrix[v.nodes.back()][closest_node.id];
        // std::cout << v.nodes.back() << " " << closest_node.id << " " << distanceMatrix[v.nodes.back()][closest_node.id] << std::endl;
        v.nodes.push_back(closest_node.id);
        gs.push_back(closest_node.id);
        nodes[closest_node.id].is_routed = true;
      }
      else{
        v.cost += distanceMatrix[v.nodes.back()][depot.id];
        // std::cout << v.nodes.back() << " " << depot.id << " " << distanceMatrix[v.nodes.back()][depot.id] << std::endl;
        v.nodes.push_back(depot.id);
        break;
      }
    }
  }
  double cost = 0;
  for(auto& v:vehicles2) cost += v.cost;
  std::cout << "Costg: " << cost << std::endl;
  for(int j=0;j<0.2*n_chromosomes;++j){
    int i = rand()%n_chromosomes;
    chromosomes[i] = gs;
    costs[i] = CalculateCost(i);
  }
  // std::cout << costs[i] << std::endl;

  // for(auto&g:gs) std::cout << g << " " ;
  // std::cout << std::endl;
  // std::cout << "OI:!!!!" <<std::endl << CalculateCost(i)<<std::endl;
}

void GANaiveSolution::RemoveSimilarSolutions(){
  std::set<int> to_delete;
  for(int i=0;i<n_chromosomes;i++){
    for(int j=i+1;j<n_chromosomes;j++){
      if(j==best) continue;
      int count = 0;
      for(int k=0;k<n_genes;k++){
        if(chromosomes[i][k]==chromosomes[j][k]) count++;
      }
      if(count > 0.95*n_genes){
        // std::cout << "These two: " << std::endl;
        // for(auto& k:chromosomes[i]){
        //   std::cout << std::setw(3) <<k ;
        // }
        // std::cout << " | " << costs[i] << std::endl;
        to_delete.insert(j);
        // for(auto& k:chromosomes[j]){
        //   std::cout << std::setw(3) <<k ;
        // }
        // std::cout << " | " << costs[j] << std::endl;
      }
    }
  }
  for(auto it = to_delete.begin();it!=to_delete.end(); ++it){
    // std::cout << "DELETED" << std::endl;
    if(rand()%100 > 75){
      chromosomes[*it] = GenerateRandomSolution();
      CalculateCost(*it);
    }

  }
}

double GANaiveSolution::CalculateCost(int i){
  int j=0;
  double cost = 0;
  while(j<n_genes){
    // std::cout << "In whle " << j<<" " << nodes[chromosomes[i][j]].demand<< std::endl;
    // std::cout << chromosomes.size() << " " << chromosomes[0].size() << std::endl;
    // std::cout << "Vehicle capacity: " << vehicle_capacity << std::endl;
    int load = vehicle_capacity - nodes[chromosomes[i][j]].demand;
    // std::cout << "0 " << chromosomes[i][j]  << std::endl;

    // std::cout << "Load: " << load << std::endl;
    cost += distanceMatrix[0][chromosomes[i][j]];
    while(j+1 < n_genes && load - nodes[chromosomes[i][j+1]].demand >=0){
      // std::cout << "------------" << std::endl;
      // std::cout << nodes[chromosomes[i][j+1]].demand << std::endl;
      load -=nodes[chromosomes[i][j+1]].demand;
      // std::cout << "Load: " << load << std::endl;
      // std::cout << "2 " << chromosomes[i][j] << " " <<chromosomes[i][j+1]<<std::endl;
      cost += distanceMatrix[chromosomes[i][j]][chromosomes[i][j+1]];
      // std::cout << chromosomes[i][j] <<" " <<chromosomes[i][j+1] << std::endl;
      j++;
      // std::cout << "3" << std::endl;
    }
    // std::cout << "out" << std::endl;
    cost += distanceMatrix[chromosomes[i][j]][0];
    // std::cout << chromosomes[i][j] <<" " << "0" << std::endl;
    j++;
  }
  // std::cout << "Returning" << std::endl;
  // exit(0);
  return cost;
}

void GANaiveSolution::CalculateTotalCost(){
  for(int i=0;i<n_chromosomes;i++) costs[i] = CalculateCost(i);
}

void GANaiveSolution::Solve(){
  GenerateRandomSolutions();
  GenerateGreedySolutions();
  CalculateTotalCost();
  best = std::min_element(costs.begin(), costs.end()) - costs.begin();

  int generation = 0;
  while(generation < generations){
    // std::cout << "Generation: " << generation << std::endl;
    NAEXCrossover();
    if(rand()%100<5) Mutate();
    CalculateTotalCost();
    if(rand()%100<5) DeleteBadChromosome();
    if(rand()%100<5) RandomSwap();
    generation++;
    // for(int i=0; i< n_chromosomes;i++){
    //   for(auto& j:chromosomes[i]){
    //     std::cout << std::setw(3) <<j ;
    //   }
    //   std::cout << " | " << costs[i] << std::endl;
    // }
    if(generation%100==0){
      // for(int i=0; i< n_chromosomes;i++){
      //   for(auto& j:chromosomes[i]){
      //     std::cout << std::setw(3) <<j ;
      //   }
      //   std::cout << " | " << costs[i] << std::endl;
      // }
      RemoveSimilarSolutions();
      // for(int i=0; i< n_chromosomes;i++){
      //   for(auto& j:chromosomes[i]){
      //     std::cout << std::setw(3) <<j ;
      //   }
      //   std::cout << " | " << costs[i] << std::endl;
      // }
    }
  }
  // for(int i=0; i< n_chromosomes;i++){
  //   for(auto& j:chromosomes[i]){
  //     std::cout << std::setw(3) <<j ;
  //   }
  //   std::cout << " | " << costs[i] << std::endl;
  // }
  GenerateBestSolution();
  // for(auto& v:vehicles) v.PrintRoute();
  double cost = 0;
  for(auto &v:vehicles) cost +=v.cost;
  std::cout << "Cost: " << cost << std::endl;
}

void GANaiveSolution::AEXCrossover(){
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
  // DeleteWorstChromosome();
  InsertionBySimilarity();
}

void GANaiveSolution::NAEXCrossover(){
  int p1 = TournamentSelection();
  int p2 = TournamentSelection();
  std::vector<int> child;
  std::unordered_set<int> reached;
  auto itp1 = &(chromosomes[p1]);
  auto itp2 = &(chromosomes[p2]);
  child.push_back((*itp1)[0]);
  reached.insert(child.back());
  int n1, n2;
  while(child.size() < n_genes){
    auto it_1 = find((*itp1).begin(), (*itp1).end(), child.back());
    auto it_2 = find((*itp2).begin(), (*itp2).end(), child.back());
    // if it = itp1.end() there is something wrong as both chromosomes should contain all the nodes
    if(it_1!= --itp1->end()
       && reached.find(*(it_1+1)) == reached.end()){
         n1 = *(it_1+1);
       }
    else{
      while(true){
        it_1++;
        if(it_1==itp1->end()) it_1 = itp1->begin();
        if(reached.find(*it_1)==reached.end()){
          n1 = *(it_1);
          break;
        }
      }
    }
    if(it_2!= --itp2->end()
       && reached.find(*(it_2+1)) == reached.end()){
         n2 = *(it_2+1);
       }
    else{
      while(true){
        it_2++;
        if(it_2==itp2->end()) it_2 = itp2->begin();
        if(reached.find(*it_2)==reached.end()){
          n2 = *(it_2);
          break;
        }
      }
    }
    // std::cout << n1 << " " << n2 << std::endl;
    if(distanceMatrix[child.back()][n1] > distanceMatrix[child.back()][n2]) std::swap(n1,n2);
    child.push_back(n1);
    reached.insert(n1);
  }
  // std::cout << "Out while" << std::endl;
  chromosomes.push_back(child);
  costs.emplace_back(CalculateCost(n_chromosomes));
  // DeleteWorstChromosome();
  InsertionBySimilarity();
  // std::cout << "Out function" << std::endl;
}

void GANaiveSolution::DeleteBadChromosome(){
  int i = TournamentSelectionBad();
  chromosomes[i] = GenerateRandomSolution();
}

int GANaiveSolution::TournamentSelection(){
  int i1 = rand()%chromosomes.size();
  int i2 = rand()%chromosomes.size();
  int i3 = rand()%chromosomes.size();
  if(costs[i1] <= costs[i2] && costs[i1] <= costs[i3]) return i1;
  if(costs[i2] <= costs[i1] && costs[i2] <= costs[i3]) return i2;
  if(costs[i3] <= costs[i1] && costs[i3] <= costs[i2]) return i3;
}

int GANaiveSolution::TournamentSelectionBad(){
  int i1 = rand()%chromosomes.size();
  int i2 = rand()%chromosomes.size();
  int i3 = rand()%chromosomes.size();
  if(costs[i1] >= costs[i2] && costs[i1] >= costs[i3]) return i1;
  if(costs[i2] >= costs[i1] && costs[i2] >= costs[i3]) return i2;
  if(costs[i3] >= costs[i1] && costs[i3] >= costs[i2]) return i3;
}

void GANaiveSolution::InsertionBySimilarity(){
  best = std::min_element(costs.begin(), costs.end()) - costs.begin();
  bool flag = true;
  for(int i=0;i<n_genes;++i){
    if(i!=best && abs(costs.back()-costs[i]) < 2*(costs[best]/100.0)){
      costs.erase(costs.begin()+i);
      chromosomes.erase(chromosomes.begin()+i);
      flag = false;
      break;
    }
  }
  if(flag) DeleteWorstChromosome();
}

void GANaiveSolution::Mutate(){
  int count =0;
  while(count<5){
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    while(r==best) r = rand()%n_chromosomes;
    int i1 = rand()%n_genes;
    int i2 = rand()%n_genes;
    if(i1>i2) std::swap(i1,i2);
    std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
    double p = costs[r];
    costs[r] = CalculateCost(r);
    if(p<costs[r]){
      std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
      count++;
      costs[r] = p;
    }
    else break;
  }
}

void GANaiveSolution::RandomSwap(){
  int count =0;
  while(count<5){
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    while(r==best) r = rand()%n_chromosomes;
    int i1 = rand()%n_genes;
    int i2 = rand()%n_genes;
    std::swap(chromosomes[r][i1],chromosomes[r][i2]);
    double p = costs[r];
    costs[r] = CalculateCost(r);
    if(p<costs[r]){
      std::swap(chromosomes[r][i1],chromosomes[r][i2]);
      count++;
      costs[r] = p;
    }
    else break;
  }
}

void GANaiveSolution::DeleteWorstChromosome(){
  auto it = std::max_element(costs.begin(), costs.end());
  int i = it - costs.begin();
  chromosomes.erase(chromosomes.begin() + i);
  costs.erase(it);
}

void GANaiveSolution::GenerateBestSolution(){
  auto it = std::min_element(costs.begin(), costs.end());
  // std::cout << "Cost: " << *it << std::endl;
  int i = it - costs.begin();
  // for(auto j:chromosomes[i]) std::cout << std::setw(3) << j;
  // std::cout << std::endl;
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
  // std::sort(chromosomes[i].begin(), chromosomes[i].end());
  // for(auto j:chromosomes[i]) std::cout << std::setw(3) << j;
}

// int main(){
//
//   Problem p;
//   GANaiveSolution g(p.nodes, p.vehicles, p.distanceMatrix, 50, 10000);
//   g.Solve();
//
//   return 0;
// }
