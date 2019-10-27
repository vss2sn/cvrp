/**
* @file genetic_algorithm.cpp
* @author vss2sn
* @brief Contiains the GASolution (Genetic Algorithm Solution) class
*/

#include "genetic_algorithm.hpp"

GASolution::GASolution(Problem p,
           int n_chromosomes,
           int generations)
           :Solution(p){
  this->n_chromosomes = n_chromosomes;
  this->generations = generations;
  n_nucleotide_pairs = nodes.size()-1;
  costs = std::vector<double>(n_chromosomes);
  n_vehicles = vehicles.size();
}

GASolution::GASolution(std::vector<Node> nodes,
           std::vector<Vehicle> vehicles,
           std::vector<std::vector<double>> distanceMatrix,
           int n_chromosomes,
           int generations)
           :Solution(nodes, vehicles, distanceMatrix){
  this->n_chromosomes = n_chromosomes;
  this->generations = generations;
  n_nucleotide_pairs = nodes.size()-1;
  costs = std::vector<double>(n_chromosomes);
  n_vehicles = vehicles.size();
}

std::vector<int> GASolution::GenerateRandomSolution(){
  std::vector<int> temp(n_nucleotide_pairs);
  for(int i=0; i<n_nucleotide_pairs; ++i){
    temp[i] = i+1;
  }
  unsigned seed = rand(); //std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(temp.begin(), temp.end(), std::default_random_engine(seed));
  return temp;
}

std::vector<int> GASolution::GenerateRandomIterSolution(){
  std::vector<int> temp(n_vehicles+1);
  std::unordered_set<int> added;
  temp[0] = 0;
  added.insert(0);
  for(int i=1; i<n_vehicles; ++i){
    int n = rand()%n_nucleotide_pairs;
    if(added.find(n)!=added.end()) n = n_nucleotide_pairs;
    temp[i] = n;
  }
  temp[n_vehicles] = n_nucleotide_pairs;
  std::sort(temp.begin(), temp.end());
  return temp;
}

void GASolution::GenerateRandomSolutions(){
  std::vector<int> temp(n_nucleotide_pairs);
  for(int i=0; i<n_nucleotide_pairs; ++i){
    temp[i] = i+1;
  }

  for(int i=0;i<n_chromosomes; i++) chromosomes.push_back(temp);
  for(int i=0;i<n_chromosomes; ++i){
    unsigned seed = rand(); //std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(chromosomes[i].begin(), chromosomes[i].end(), std::default_random_engine(seed));
  }
  for(int j=0;j<n_chromosomes;j++){
    std::vector<int> temp_i(n_vehicles+1);
    std::unordered_set<int> added;
    temp_i[0]=0;
    for(int i=1; i<n_vehicles; ++i){
      int n = rand()%n_nucleotide_pairs;
      if(added.find(n)!=added.end()) n = n_nucleotide_pairs;
      temp_i[i] = n;
    }
    temp_i[n_vehicles] = n_nucleotide_pairs;
    std::sort(temp_i.begin(), temp_i.end());
    iterators.push_back(temp_i);
  }
}

void GASolution::GenerateGreedySolutions(){
  std::vector<int> gs;
  auto vehicles2 = vehicles;
  std::vector<int> iter;
  iter.push_back(0);
  for(auto& v:vehicles2){
    while(true){
      Node closest_node = find_closest(v, distanceMatrix, nodes);
      if(closest_node.id!=-1 && v.load - closest_node.demand >=0){
        v.load -= closest_node.demand;
        v.cost += distanceMatrix[v.nodes.back()][closest_node.id];
        v.nodes.push_back(closest_node.id);
        gs.push_back(closest_node.id);
        nodes[closest_node.id].is_routed = true;
      }
      else{
        iter.push_back(iter.back() + v.nodes.size()-1);
        v.cost += distanceMatrix[v.nodes.back()][depot.id];
        v.nodes.push_back(depot.id);
        break;
      }
    }
  }
  double cost = 0;
  for(auto& v:vehicles2) cost += v.cost;
  if(gs.size() != n_nucleotide_pairs){
    std::cout << "Initial solution does not contain all the nodes. Exiting" << std::endl;
    exit(0);
  }
  chromosomes[0] = gs;
  iterators[0] = iter;
  costs[0] = CalculateCost(0);
  for(int j=1;j<0.2*n_chromosomes;++j){
    gs.clear();
    iter.clear();
    vehicles2 = vehicles;
    iter.push_back(0);
    for(auto& n:nodes) n.is_routed = false;
    nodes[depot.id].is_routed = true;
    int count=0;
    for(auto& v:vehicles2){
      while(true){
        Node closest_node;
        if(count==0){
          int i = rand()%(n_nucleotide_pairs-1)+1;
          closest_node = nodes[i];
          count++;
        }
        else closest_node = find_closest(v, distanceMatrix, nodes);
        if(closest_node.id!=-1 && v.load - closest_node.demand >=0){
          v.load -= closest_node.demand;
          v.cost += distanceMatrix[v.nodes.back()][closest_node.id];
          v.nodes.push_back(closest_node.id);
          gs.push_back(closest_node.id);
          nodes[closest_node.id].is_routed = true;
        }
        else{
          iter.push_back(iter.back() + v.nodes.size()-1);
          v.cost += distanceMatrix[v.nodes.back()][depot.id];
          v.nodes.push_back(depot.id);
          break;
        }
      }
    }
    chromosomes[j] = gs;
    if(gs.size() != n_nucleotide_pairs){
      std::cout << "Initial solutions do not contain all the nodes. Exiting" << std::endl;
      exit(0);
    }
    iterators[j] = iter;
    MakeValid(j);
    // if(!checkValidity(j)) std::cout << "Oi"<<std::endl;
    costs[j] = CalculateCost(j);
  }
}

void GASolution::RemoveSimilarSolutions(){
  std::set<int> to_delete;
  for(int i=0;i<n_chromosomes;i++){
    for(int j=0;j<n_chromosomes;j++){
      if(j==i) continue;
      if(j==best) continue;
      int count = 0;
      for(int k=0;k<n_nucleotide_pairs;k++){
        if(chromosomes[i][k]==chromosomes[j][k]) count++;
      }
      if(count > 0.95*n_nucleotide_pairs &&
        ((costs[i]>0.95*costs[j] && costs[i]<1.05*costs[j]) ||
         (costs[j]>0.95*costs[i] && costs[j]<1.05*costs[i]))
      ){
        if(costs[i]>costs[j])to_delete.insert(i);
        else to_delete.insert(j);
      }
    }
  }
  for(auto it = to_delete.begin();it!=to_delete.end(); ++it){
    if(rand()%100 > 15){
      chromosomes[*it] = GenerateRandomSolution();
      iterators[*it] = GenerateRandomIterSolution();
      MakeValid(*it);
      // if(!checkValidity(*it)) std::cout << "OI"<< std::endl;
      costs[*it] = CalculateCost(*it);
    }
  }
}

double GASolution::CalculateCost(int i){
  double cost = 0;
  double tmp = costs[i];
  for(int k=0;k<iterators[0].size()-1;k++){
    if(iterators[i][k]==n_nucleotide_pairs) break;
    int j=iterators[i][k];
    if(j<iterators[i][k+1]) cost+=distanceMatrix[0][chromosomes[i][j]];
    while(j+1<iterators[i][k+1]){
      cost+=distanceMatrix[chromosomes[i][j]][chromosomes[i][j+1]];
      j++;
    }
    cost+=distanceMatrix[chromosomes[i][j]][0];
  }
  return cost;
}

void GASolution::CalculateTotalCost(){
  for(int i=0;i<n_chromosomes;i++){
    double tmp = costs[i];
    costs[i] = CalculateCost(i);
    if(costs[i]>tmp){

    }
  }
}

void GASolution::Solve(){
  GenerateRandomSolutions();
  for(int i=0;i<n_chromosomes;i++) MakeValid(i);
  GenerateGreedySolutions();
  CalculateTotalCost();
  best = std::min_element(costs.begin(), costs.end()) - costs.begin();
  int generation = 0;
  while(generation < generations){
    // std::cout << "Generation: " << generation << std::endl;
    // for(int i=0;i<chromosomes.size();i++){
    //   if(!checkValidity(i)) std::cout << "Invalid" << std::endl;
    // }
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    if(rand()%2==0){
      HGreXCrossover();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    }
    if(rand()%2==0){
      int n = rand()%n_chromosomes;
      auto temp_i = iterators[n];
      MutateIterLeft(n, rand()%n_vehicles);
      double c = CalculateCost(n);
      if(c<costs[n]) costs[n]=c;
      else iterators[n]=temp_i;
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    }
    else{
      int n = rand()%n_chromosomes;
      auto temp_i = iterators[n];
      MutateIterRight(n, rand()%n_vehicles);
      double c = CalculateCost(n);
      if(c<costs[n]) costs[n]=c;
      else iterators[n]=temp_i;
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    }
    if(rand()%100<50){
      Mutate();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    }
    if(rand()%100<50) {
      RandomSwap();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    }
    if(rand()%100<50) {
      MutateWhithinGene();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    }
    if(rand()%100<70) {
      InsertIterDist();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    }
    // if(rand()%100<5) {
    //   AddBest();
    //   best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    // }
    // if(rand()%100<20) {
    //   DeleteBadChromosome();
    // }
    CalculateTotalCost();
    generation++;
    // if(generation%100==0){
    //   RemoveSimilarSolutions();
    // }
  }
  GenerateBestSolution();
}

void GASolution::HGreXCrossover(){
  int p1 = TournamentSelection();
  int p2 = TournamentSelection();
  std::vector<int> child;
  std::unordered_set<int> reached;
  auto itp1 = &(chromosomes[p1]);
  auto itp2 = &(chromosomes[p2]);
  child.push_back((*itp1)[0]);
  reached.insert(child.back());
  int n1, n2;
  while(child.size() < n_nucleotide_pairs){
    auto it_1 = find((*itp1).begin(), (*itp1).end(), child.back());
    auto it_2 = find((*itp2).begin(), (*itp2).end(), child.back());
    // if it = itp1.end() there is something wrong as both chromosomes should contain all the nodes
    if(it_1!= itp1->end()-1
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
    if(it_2!= itp2->end()-1
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
    if(distanceMatrix[child.back()][n1] > distanceMatrix[child.back()][n2]) std::swap(n1,n2);
    child.push_back(n1);
    reached.insert(n1);
  }
  chromosomes.push_back(child);
  int temp = rand()%100;
  if(temp < 20)iterators.emplace_back(GenerateRandomIterSolution());
  else if(temp < 60)iterators.emplace_back(iterators[p1]);
  else iterators.emplace_back(iterators[p2]);
  MakeValid(n_chromosomes);
  if(checkValidity(n_chromosomes)){
    costs.emplace_back(CalculateCost(n_chromosomes));
    InsertionBySimilarity();
  }
  else{
    iterators.erase(iterators.begin()+n_chromosomes-1);
    chromosomes.erase(chromosomes.begin()+n_chromosomes-1);
  }
}


//Works if and only if a solution is possible. No check on validity after function executes
void GASolution::MakeValid(int i){
  for(int j=0;j<n_vehicles-1;j++){
    int load = capacity;
    int iter = iterators[i][j];
    while(iter < iterators[i][j+1]){
      load-=nodes[chromosomes[i][iter]].demand;
      ++iter;
    }
    if(load<0){
      iterators[i][j+1]--;
      j--;
    };
  }

  for(int j=n_vehicles;j>1;j--){
    int load = capacity;
    int iter = iterators[i][j]-1;
    while(iter >= iterators[i][j-1]){
      load-=nodes[chromosomes[i][iter]].demand;
      --iter;
    }
    if(load<0){
      iterators[i][j-1]++;
      j++;
    }
  }
}

void GASolution::DeleteBadChromosome(){
  int i = TournamentSelectionBad();
  chromosomes[i] = GenerateRandomSolution();
}

int GASolution::TournamentSelection(){
  int i1 = rand()%chromosomes.size();
  int i2 = rand()%chromosomes.size();
  int i3 = rand()%chromosomes.size();
  if(costs[i1] <= costs[i2] && costs[i1] <= costs[i3]) return i1;
  if(costs[i2] <= costs[i1] && costs[i2] <= costs[i3]) return i2;
  if(costs[i3] <= costs[i1] && costs[i3] <= costs[i2]) return i3;
}

int GASolution::TournamentSelectionBad(){
  int i1 = rand()%chromosomes.size();
  int i2 = rand()%chromosomes.size();
  int i3 = rand()%chromosomes.size();
  if(costs[i1] >= costs[i2] && costs[i1] >= costs[i3]) return i1;
  if(costs[i2] >= costs[i1] && costs[i2] >= costs[i3]) return i2;
  if(costs[i3] >= costs[i1] && costs[i3] >= costs[i2]) return i3;
}

void GASolution::InsertionBySimilarity(){
  best = std::min_element(costs.begin(), costs.end()) - costs.begin();
  bool flag = true;
  for(int i=0;i<n_nucleotide_pairs;++i){
    if(i!=best && costs.back()-costs[i] < 2*(costs[best]/100.0)){
      costs.erase(costs.begin()+i);
      chromosomes.erase(chromosomes.begin()+i);
      iterators.erase(iterators.begin()+i);
      flag = false;
      break;
    }
  }
  if(flag) DeleteRandomChromosome();
}

void GASolution::DeleteRandomChromosome(){
  int r = rand()%n_chromosomes;
  while(r==best) r = rand()%n_chromosomes;
  chromosomes[r] = chromosomes.back();
  iterators[r] = iterators.back();
  chromosomes.erase(chromosomes.begin()+chromosomes.size()-1);
  iterators.erase(iterators.begin()+iterators.size()-1);
}

void GASolution::Mutate(){
  int count =0;
  while(count<20){
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    // while(r==best) r = rand()%n_chromosomes;
    int i1 = rand()%n_nucleotide_pairs;
    int i2 = rand()%n_nucleotide_pairs;
    if(i1>i2) std::swap(i1,i2);
    auto temp_it = iterators[r];
    std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
    MakeValid(r);
    double p = costs[r];
    costs[r] = CalculateCost(r);
    if(p<costs[r]){
      std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
      iterators[r] = temp_it;
      count++;
      costs[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

void GASolution::SwapWhithinGene(){
  int count =0;
  while(count<20){
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    // while(r==best) r = rand()%n_chromosomes;
    int v = rand()%n_vehicles;
    int delta = iterators[r][v+1]-iterators[r][v];
    if(delta<1) return;
    int i1 = iterators[r][v] + rand()%delta;
    int i2 = iterators[r][v] + rand()%delta;
    std::swap(chromosomes[r][i1], chromosomes[r][i2]);
    auto temp_it = iterators[r];
    MakeValid(r);
    double p = costs[r];
    costs[r] = CalculateCost(r);
    if(p<costs[r]){
      std::swap(chromosomes[r][i1], chromosomes[r][i2]);
      iterators[r] = temp_it;
      count++;
      costs[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

void GASolution::MutateWhithinGene(){
  int count =0;
  while(count<20){
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    while(r==best) r = rand()%n_chromosomes;
    int v = rand()%n_vehicles;
    int delta = iterators[r][v+1]-iterators[r][v];
    if(delta<1) return;
    int i1 = iterators[r][v] + rand()%delta;
    int i2 = iterators[r][v] + rand()%delta;
    if(i1>i2) std::swap(i1,i2);
    std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
    auto temp_it = iterators[r];
    MakeValid(r);
    double p = costs[r];
    costs[r] = CalculateCost(r);
    if(p<costs[r]){
      std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
      iterators[r] = temp_it;
      count++;
      costs[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

bool GASolution::MutateIterLeft(int i_chromosome, int j_in){
  if(j_in==n_vehicles || j_in==0) return false;

  int i = i_chromosome;
  auto temp = iterators[i][j_in];

  if(iterators[i][j_in] > iterators[i][j_in-1])iterators[i][j_in]--;
  for(int j=j_in;j<n_vehicles-1;j++){
    int load = capacity;
    int iter = iterators[i][j];
    while(iter < iterators[i][j+1]){
      load-=nodes[chromosomes[i][iter]].demand;
      ++iter;
    }
    if(load<0){
      iterators[i][j+1]--;
      j--;
    }
  }

  for(int j=n_vehicles;j>1;j--){
    int load = capacity;
    int iter = iterators[i][j]-1;
    while(iter >= iterators[i][j-1]){
      load-=nodes[chromosomes[i][iter]].demand;
      --iter;
    }
    if(load<0){
      iterators[i][j-1]++;
      j++;
    }
  }
  return true;
}

bool GASolution::MutateIterRight(int i_chromosome, int j_in){
  if(j_in==n_vehicles || j_in==0) return false;

  int i = i_chromosome;
  auto temp = iterators[i][j_in];

  if(iterators[i][j_in] < iterators[i][j_in-1])iterators[i][j_in]++;

  for(int j=j_in;j>1;j--){
    int load = capacity;
    int iter = iterators[i][j]-1;
    while(iter >= iterators[i][j-1]){
      load-=nodes[chromosomes[i][iter]].demand;
      --iter;
    }
    if(load<0){
      iterators[i][j-1]++;
      j++;
    }
  }

  for(int j=0;j<n_vehicles-1;j++){
    int load = capacity;
    int iter = iterators[i][j];
    while(iter < iterators[i][j+1]){
      load-=nodes[chromosomes[i][iter]].demand;
      ++iter;
    }
    if(load<0){
      iterators[i][j+1]--;
      j--;
    }
  }
  return true;
}

bool GASolution::checkValidity(int i){
  for(int j=0;j<n_vehicles;j++){
    int load = capacity;
    int iter = iterators[i][j];
    while(iter < iterators[i][j+1]){
      load-=nodes[chromosomes[i][iter]].demand;
      ++iter;
    }
    if(load<0) return false;
  }
  return true;
}

void GASolution::RandomSwap(){
  int count =0;
  while(count<20){
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    // while(r==best) r = rand()%n_chromosomes;
    int i1 = rand()%n_nucleotide_pairs;
    int i2 = rand()%n_nucleotide_pairs;
    std::swap(chromosomes[r][i1],chromosomes[r][i2]);
    auto temp_it = iterators[r];
    MakeValid(r);
    double p = costs[r];
    costs[r] = CalculateCost(r);
    if(p<costs[r]){
      std::swap(chromosomes[r][i1],chromosomes[r][i2]);
      iterators[r] = temp_it;
      count++;
      costs[r] = p;
    }
    else if(checkValidity(r)) break;
  }
}

void GASolution::AddBest(){
  best = std::min_element(costs.begin(), costs.end()) - costs.begin();
  int worst = std::min_element(costs.begin(), costs.end()) - costs.begin();
  chromosomes[worst] = chromosomes[best];
  costs[worst] = costs[best];
  iterators[worst] = iterators[best];
}

void GASolution::DeleteWorstChromosome(){
  auto it = std::max_element(costs.begin(), costs.end());
  int i = it - costs.begin();
  chromosomes.erase(chromosomes.begin() + i);
  costs.erase(it);
  iterators.erase(iterators.begin() + i);
}

void GASolution::InsertIterDist(){
  // std::cout << "1"<<std::endl;
  int n = rand()%n_chromosomes;
  auto temp = iterators[n];
  int j = n_vehicles;
  while(iterators[n][j]==n_nucleotide_pairs) j--;
  if(j==n_vehicles-1) return;
  j++;
  //that found the ierator to insert
  double cost = 0;
  int iter_begin;
  int range = 0;
  for(int i=0;i<n_vehicles;i++){
    int c = 0;
    c += distanceMatrix[0][iterators[n][i]];
    for(int k=iterators[n][i];k<iterators[n][i+1]-1;k++){
      c += distanceMatrix[chromosomes[n][k]][chromosomes[n][k+1]];
    }
    if(iterators[n][i+1]-iterators[n][i]<2) continue;
    c += distanceMatrix[iterators[n][i+1]-1][0];
    if(c>cost){
      cost = c;
      iter_begin = i;
      range = iterators[n][i+1]-iterators[n][i];
    }
  }
  int i = iter_begin;
  if(cost = 0 || range<2) return;
  int val = iterators[n][i]+rand()%(range-1)+1;
  iterators[n].erase(iterators[n].begin()+j);
  iterators[n].insert(iterators[n].begin()+i+1, val);
  MakeValid(n); // dont  htink this is req
  if(!checkValidity(n)) std::cout << "Invalid from insertiterdist"<<std::endl;
  int c2 = CalculateCost(n);
  if(costs[n]<c2) iterators[n] = temp;
  else costs[n] = c2;
}

void GASolution::GenerateBestSolution(){
  auto it = std::min_element(costs.begin(), costs.end());
  std::cout << "Cost: " << *it << std::endl;
  int i = it-costs.begin();
  // for(auto& m:chromosomes[i])std::cout << m << " ";
  // std::cout << std::endl;
  // for(auto& m:iterators[i])std::cout << m << " ";
  // std::cout << std::endl;

  // std::cout << "Valid" << checkValidity(i) << std::endl;
  auto v = vehicles.begin();
  for(int k=0;k<iterators[0].size()-1;k++, v++){
    v->cost = 0;
    if(iterators[i][k]==n_nucleotide_pairs) break;
    int j=iterators[i][k];
    if(j<iterators[i][k+1]){
      v->cost+=distanceMatrix[0][chromosomes[i][j]];
      v->nodes.push_back(chromosomes[i][j]);
      v->load -= nodes[chromosomes[i][j]].demand;
    }
    while(j+1<iterators[i][k+1]){
      v->cost+=distanceMatrix[chromosomes[i][j]][chromosomes[i][j+1]];
      v->nodes.push_back(chromosomes[i][j+1]);
      v->load -= nodes[chromosomes[i][j+1]].demand;
      j++;
    }
    v->cost += distanceMatrix[v->nodes.back()][depot.id];
    v->nodes.push_back(depot.id);
  }
  while(v!=vehicles.end()){
    v->cost = 0;
    ++v;
  }
  // for(auto& v2:vehicles) v2.PrintRoute();
  std::cout << "Solution valid: " << CheckSolutionValid()<< std::endl;
}
