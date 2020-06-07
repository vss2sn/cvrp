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
  this->n_chromosomes_ = n_chromosomes;
  this->generations_ = generations;
  n_nucleotide_pairs_ = nodes_.size()-1;
  costs_ = std::vector<double>(n_chromosomes);
  n_vehicles_ = vehicles_.size();

  GenerateRandomSolutions();
  for(int i=0;i<n_chromosomes;i++) MakeValid(i);
  GenerateGreedySolutions();
  CalculateTotalCost();
  best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
}

GASolution::GASolution(Solution s,
           int n_chromosomes,
           int generations)
           :Solution(s){
  this->n_chromosomes_ = n_chromosomes;
  this->generations_ = generations;
  n_nucleotide_pairs_ = nodes_.size()-1;
  costs_ = std::vector<double>(n_chromosomes);
  n_vehicles_ = vehicles_.size();

  std::vector<int> temp_c, temp_i;
  temp_i.push_back(0);
  for(auto& v:vehicles_){
    for(size_t i=1;i<v.nodes_.size()-1;i++) temp_c.push_back(v.nodes_[i]);
    temp_i.push_back(temp_c.size());
  }

  // Reset vehicles_ to allow GenerateGreedySolutions() to run currectly
  for(auto& v:vehicles_){
    v.nodes_.clear();
    v.nodes_.push_back(0);
    v.load_ = capacity_;
  }

  for(auto& n:nodes_) n.is_routed_ = false;
  nodes_[0].is_routed_ = true;

  GenerateRandomSolutions();
  for(int i=0;i<n_chromosomes;i++) MakeValid(i);
  GenerateGreedySolutions();
  // Replacing the greedy solution (1st chromosome) with the solution given as input
  chromosomes_[0] = temp_c;
  iterators_[0] = temp_i;
  if(!checkValidity(0) || chromosomes_[0].size() != size_t(n_nucleotide_pairs_)){
    // Extra sanity check for size of solution
    std::cout << "The input solution is invalid. Exiting." <<std::endl;
    exit(0);
  }
  CalculateTotalCost();
  best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
}

GASolution::GASolution(std::vector<Node> nodes,
           std::vector<Vehicle> vehicles_,
           std::vector<std::vector<double>> distanceMatrix,
           int n_chromosomes,
           int generations)
           :Solution(nodes, vehicles_, distanceMatrix){
  this->n_chromosomes_ = n_chromosomes;
  this->generations_ = generations;
  n_nucleotide_pairs_ = nodes_.size()-1;
  costs_ = std::vector<double>(n_chromosomes);
  n_vehicles_ = vehicles_.size();

  GenerateRandomSolutions();
  for(int i=0;i<n_chromosomes;i++) MakeValid(i);
  GenerateGreedySolutions();
  CalculateTotalCost();
  best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
}

std::vector<int> GASolution::GenerateRandomSolution(){
  std::vector<int> temp(n_nucleotide_pairs_);
  for(int i=0; i<n_nucleotide_pairs_; ++i){
    temp[i] = i+1;
  }
  unsigned seed = rand(); //std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(temp.begin(), temp.end(), std::default_random_engine(seed));
  return temp;
}

std::vector<int> GASolution::GenerateRandomIterSolution(){
  std::vector<int> temp(n_vehicles_+1);
  std::unordered_set<int> added;
  temp[0] = 0;
  added.insert(0);
  for(int i=1; i<n_vehicles_; ++i){
    int n = rand()%n_nucleotide_pairs_;
    if(added.find(n)!=added.end()) n = n_nucleotide_pairs_;
    temp[i] = n;
  }
  temp[n_vehicles_] = n_nucleotide_pairs_;
  std::sort(temp.begin(), temp.end());
  return temp;
}

void GASolution::GenerateRandomSolutions(){
  std::vector<int> temp(n_nucleotide_pairs_);
  for(int i=0; i<n_nucleotide_pairs_; ++i){
    temp[i] = i+1;
  }

  for(int i=0;i<n_chromosomes_; i++) chromosomes_.push_back(temp);
  for(int i=0;i<n_chromosomes_; ++i){
    unsigned seed = rand(); //std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(chromosomes_[i].begin(), chromosomes_[i].end(), std::default_random_engine(seed));
  }
  for(int j=0;j<n_chromosomes_;j++){
    std::vector<int> temp_i(n_vehicles_+1);
    std::unordered_set<int> added;
    temp_i[0]=0;
    for(int i=1; i<n_vehicles_; ++i){
      int n = rand()%n_nucleotide_pairs_;
      if(added.find(n)!=added.end()) n = n_nucleotide_pairs_;
      temp_i[i] = n;
    }
    temp_i[n_vehicles_] = n_nucleotide_pairs_;
    std::sort(temp_i.begin(), temp_i.end());
    iterators_.push_back(temp_i);
  }
}

void GASolution::GenerateGreedySolutions(){
  std::vector<int> gs;
  auto vehicles_2 = vehicles_;
  std::vector<int> iter;
  iter.push_back(0);
  for(auto& v:vehicles_2){
    while(true){
      Node closest_node = find_closest(v);
      if(closest_node.id_!=-1 && v.load_ - closest_node.demand_ >=0){
        v.load_ -= closest_node.demand_;
        v.cost_ += distanceMatrix_[v.nodes_.back()][closest_node.id_];
        v.nodes_.push_back(closest_node.id_);
        gs.push_back(closest_node.id_);
        nodes_[closest_node.id_].is_routed_ = true;
      }
      else{
        iter.push_back(iter.back() + v.nodes_.size()-1);
        v.cost_ += distanceMatrix_[v.nodes_.back()][depot_.id_];
        v.nodes_.push_back(depot_.id_);
        break;
      }
    }
  }
  double cost = 0;
  for(auto& v:vehicles_2) cost += v.cost_;
  if(gs.size() != size_t(n_nucleotide_pairs_)){
    std::cout << "Initial solution does not contain all the nodes_. Exiting" << std::endl;
    exit(0);
  }
  chromosomes_[0] = gs;
  iterators_[0] = iter;
  costs_[0] = CalculateCost(0);
  for(int j=1;j<0.2*n_chromosomes_;++j){
    gs.clear();
    iter.clear();
    vehicles_2 = vehicles_;
    iter.push_back(0);
    for(auto& n:nodes_) n.is_routed_ = false;
    nodes_[depot_.id_].is_routed_ = true;
    int count=0;
    for(auto& v:vehicles_2){
      while(true){
        Node closest_node;
        if(count==0){
          int i = rand()%(n_nucleotide_pairs_-1)+1;
          closest_node = nodes_[i];
          count++;
        }
        else closest_node = find_closest(v);
        if(closest_node.id_!=-1 && v.load_ - closest_node.demand_ >=0){
          v.load_ -= closest_node.demand_;
          v.cost_ += distanceMatrix_[v.nodes_.back()][closest_node.id_];
          v.nodes_.push_back(closest_node.id_);
          gs.push_back(closest_node.id_);
          nodes_[closest_node.id_].is_routed_ = true;
        }
        else{
          iter.push_back(iter.back() + v.nodes_.size()-1);
          v.cost_ += distanceMatrix_[v.nodes_.back()][depot_.id_];
          v.nodes_.push_back(depot_.id_);
          break;
        }
      }
    }
    chromosomes_[j] = gs;
    if(gs.size() != size_t(n_nucleotide_pairs_)){
      std::cout << "Initial solutions do not contain all the nodes_. Exiting" << std::endl;
      exit(0);
    }
    iterators_[j] = iter;
    MakeValid(j);
    costs_[j] = CalculateCost(j);
  }
}

void GASolution::RemoveSimilarSolutions(){
  std::set<int> to_delete;
  for(int i=0;i<n_chromosomes_;i++){
    for(int j=0;j<n_chromosomes_;j++){
      if(j==i) continue;
      if(j==best_) continue;
      int count = 0;
      for(int k=0;k<n_nucleotide_pairs_;k++){
        if(chromosomes_[i][k]==chromosomes_[j][k]) count++;
      }
      if(count > 0.95*n_nucleotide_pairs_ &&
        ((costs_[i]>0.95*costs_[j] && costs_[i]<1.05*costs_[j]) ||
         (costs_[j]>0.95*costs_[i] && costs_[j]<1.05*costs_[i]))
      ){
        if(costs_[i]>costs_[j])to_delete.insert(i);
        else to_delete.insert(j);
      }
    }
  }
  for(auto it = to_delete.begin();it!=to_delete.end(); ++it){
    if(rand()%100 > 15){
      chromosomes_[*it] = GenerateRandomSolution();
      iterators_[*it] = GenerateRandomIterSolution();
      MakeValid(*it);
      costs_[*it] = CalculateCost(*it);
    }
  }
}

double GASolution::CalculateCost(int i){
  double cost = 0;
  for(size_t k=0;k<iterators_[0].size()-1;k++){
    if(iterators_[i][k]==n_nucleotide_pairs_) break;
    int j=iterators_[i][k];
    if(j<iterators_[i][k+1]) cost+=distanceMatrix_[0][chromosomes_[i][j]];
    while(j+1<iterators_[i][k+1]){
      cost+=distanceMatrix_[chromosomes_[i][j]][chromosomes_[i][j+1]];
      j++;
    }
    cost+=distanceMatrix_[chromosomes_[i][j]][0];
  }
  return cost;
}

void GASolution::CalculateTotalCost(){
  for(int i=0;i<n_chromosomes_;i++){
    // double tmp = costs_[i];
    costs_[i] = CalculateCost(i);
  }
}

void GASolution::Solve(){
  int generation = 0;
  while(generation < generations_){
    // std::cout << "Generation: " << generation << std::endl;
    // for(int i=0;i<chromosomes_.size();i++){
    //   if(!checkValidity(i)) std::cout << "Invalid" << std::endl;
    // }
    best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    if(rand()%2==0){
      HGreXCrossover();
      best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    }
    if(rand()%2==0){
      int n = rand()%n_chromosomes_;
      auto temp_i = iterators_[n];
      MutateIterLeft(n, rand()%n_vehicles_);
      double c = CalculateCost(n);
      if(c<costs_[n]) costs_[n]=c;
      else iterators_[n]=temp_i;
      best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    }
    else{
      int n = rand()%n_chromosomes_;
      auto temp_i = iterators_[n];
      MutateIterRight(n, rand()%n_vehicles_);
      double c = CalculateCost(n);
      if(c<costs_[n]) costs_[n]=c;
      else iterators_[n]=temp_i;
      best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    }
    if(rand()%100<50){
      Mutate();
      best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    }
    if(rand()%100<50) {
      RandomSwap();
      best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    }
    if(rand()%100<50) {
      MutateWhithinGene();
      best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    }
    if(rand()%100<70) {
      InsertIterDist();
      best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    }
    // if(rand()%100<5) {
    //   Addbest();
    //   best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    // }
    // if(rand()%100<20) {
    //   DeleteBadChromosome();
    // }
    CalculateTotalCost();
    generation++;
    // if(generation%100==0){
    //   RemoveSimilarSolutions();
    // }

    solution_string_ = std::to_string(depot_.id_);
    for(size_t k=0;k<iterators_[0].size()-1;k++){
      if(iterators_[best_][k]==n_nucleotide_pairs_) break;
      int j=iterators_[best_][k];
      if(j<iterators_[best_][k+1]){
        solution_string_ += ',' + std::to_string(chromosomes_[best_][j]);
      }
      while(j+1<iterators_[best_][k+1]){
        solution_string_ += ',' + std::to_string(chromosomes_[best_][j+1]);
        j++;
      }
      solution_string_ += ',' + std::to_string(depot_.id_);
    }

#ifdef VISUALIZE
      mpp->publishToRViz(solution_string_, nodes_);;
#endif // VISUALIZE

  }
  GenerateBestSolution();
}

void GASolution::HGreXCrossover(){
  int p1 = TournamentSelection();
  int p2 = TournamentSelection();
  std::vector<int> child;
  std::unordered_set<int> reached;
  auto itp1 = &(chromosomes_[p1]);
  auto itp2 = &(chromosomes_[p2]);
  child.push_back((*itp1)[0]);
  reached.insert(child.back());
  int n1, n2;
  while(child.size() < size_t(n_nucleotide_pairs_)){
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
    if(distanceMatrix_[child.back()][n1] > distanceMatrix_[child.back()][n2]) std::swap(n1,n2);
    child.push_back(n1);
    reached.insert(n1);
  }
  chromosomes_.push_back(child);
  int temp = rand()%100;
  if(temp < 20)iterators_.emplace_back(GenerateRandomIterSolution());
  else if(temp < 60)iterators_.emplace_back(iterators_[p1]);
  else iterators_.emplace_back(iterators_[p2]);
  MakeValid(n_chromosomes_);
  if(checkValidity(n_chromosomes_)){
    costs_.emplace_back(CalculateCost(n_chromosomes_));
    InsertionBySimilarity();
  }
  else{
    iterators_.erase(iterators_.begin()+n_chromosomes_-1);
    chromosomes_.erase(chromosomes_.begin()+n_chromosomes_-1);
  }
}


//Works if and only if a solution is possible. No check on validity after function executes
void GASolution::MakeValid(int i){
  for(int j=0;j<n_vehicles_-1;j++){
    int load = capacity_;
    int iter = iterators_[i][j];
    while(iter < iterators_[i][j+1]){
      load-=nodes_[chromosomes_[i][iter]].demand_;
      ++iter;
    }
    if(load<0){
      iterators_[i][j+1]--;
      j--;
    };
  }

  for(int j=n_vehicles_;j>1;j--){
    int load = capacity_;
    int iter = iterators_[i][j]-1;
    while(iter >= iterators_[i][j-1]){
      load-=nodes_[chromosomes_[i][iter]].demand_;
      --iter;
    }
    if(load<0){
      iterators_[i][j-1]++;
      j++;
    }
  }
}

void GASolution::DeleteBadChromosome(){
  int i = TournamentSelectionBad();
  chromosomes_[i] = GenerateRandomSolution();
}

int GASolution::TournamentSelection(){
  int i1 = rand()%chromosomes_.size();
  int i2 = rand()%chromosomes_.size();
  int i3 = rand()%chromosomes_.size();
  if(costs_[i1] <= costs_[i2] && costs_[i1] <= costs_[i3]) return i1;
  else if(costs_[i2] <= costs_[i1] && costs_[i2] <= costs_[i3]) return i2;
  else return i3;
}

int GASolution::TournamentSelectionBad(){
  int i1 = rand()%chromosomes_.size();
  int i2 = rand()%chromosomes_.size();
  int i3 = rand()%chromosomes_.size();
  if(costs_[i1] >= costs_[i2] && costs_[i1] >= costs_[i3]) return i1;
  else if(costs_[i2] >= costs_[i1] && costs_[i2] >= costs_[i3]) return i2;
  else return i3;
}

void GASolution::InsertionBySimilarity(){
  best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
  bool flag = true;
  for(int i=0;i<n_nucleotide_pairs_;++i){
    if(i!=best_ && costs_.back()-costs_[i] < 2*(costs_[best_]/100.0)){
      costs_.erase(costs_.begin()+i);
      chromosomes_.erase(chromosomes_.begin()+i);
      iterators_.erase(iterators_.begin()+i);
      flag = false;
      break;
    }
  }
  if(flag) DeleteRandomChromosome();
}

void GASolution::DeleteRandomChromosome(){
  int r = rand()%n_chromosomes_;
  while(r==best_) r = rand()%n_chromosomes_;
  chromosomes_[r] = chromosomes_.back();
  iterators_[r] = iterators_.back();
  chromosomes_.erase(chromosomes_.begin()+chromosomes_.size()-1);
  iterators_.erase(iterators_.begin()+iterators_.size()-1);
}

void GASolution::Mutate(){
  int count =0;
  while(count<20){
    best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    int r = rand()%n_chromosomes_;
    // while(r==best_) r = rand()%n_chromosomes_;
    int i1 = rand()%n_nucleotide_pairs_;
    int i2 = rand()%n_nucleotide_pairs_;
    if(i1>i2) std::swap(i1,i2);
    auto temp_it = iterators_[r];
    std::reverse(chromosomes_[r].begin() + i1, chromosomes_[r].begin() + i2);
    MakeValid(r);
    double p = costs_[r];
    costs_[r] = CalculateCost(r);
    if(p<costs_[r]){
      std::reverse(chromosomes_[r].begin() + i1, chromosomes_[r].begin() + i2);
      iterators_[r] = temp_it;
      count++;
      costs_[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

void GASolution::SwapWhithinGene(){
  int count =0;
  while(count<20){
    best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    int r = rand()%n_chromosomes_;
    // while(r==best_) r = rand()%n_chromosomes_;
    int v = rand()%n_vehicles_;
    int delta = iterators_[r][v+1]-iterators_[r][v];
    if(delta<1) return;
    int i1 = iterators_[r][v] + rand()%delta;
    int i2 = iterators_[r][v] + rand()%delta;
    std::swap(chromosomes_[r][i1], chromosomes_[r][i2]);
    auto temp_it = iterators_[r];
    MakeValid(r);
    double p = costs_[r];
    costs_[r] = CalculateCost(r);
    if(p<costs_[r]){
      std::swap(chromosomes_[r][i1], chromosomes_[r][i2]);
      iterators_[r] = temp_it;
      count++;
      costs_[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

void GASolution::MutateWhithinGene(){
  int count =0;
  while(count<20){
    best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    int r = rand()%n_chromosomes_;
    while(r==best_) r = rand()%n_chromosomes_;
    int v = rand()%n_vehicles_;
    int delta = iterators_[r][v+1]-iterators_[r][v];
    if(delta<1) return;
    int i1 = iterators_[r][v] + rand()%delta;
    int i2 = iterators_[r][v] + rand()%delta;
    if(i1>i2) std::swap(i1,i2);
    std::reverse(chromosomes_[r].begin() + i1, chromosomes_[r].begin() + i2);
    auto temp_it = iterators_[r];
    MakeValid(r);
    double p = costs_[r];
    costs_[r] = CalculateCost(r);
    if(p<costs_[r]){
      std::reverse(chromosomes_[r].begin() + i1, chromosomes_[r].begin() + i2);
      iterators_[r] = temp_it;
      count++;
      costs_[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

bool GASolution::MutateIterLeft(int i_chromosome, int j_in){
  if(j_in==n_vehicles_ || j_in==0) return false;

  int i = i_chromosome;
  if(iterators_[i][j_in] > iterators_[i][j_in-1])iterators_[i][j_in]--;
  for(int j=j_in;j<n_vehicles_-1;j++){
    int load = capacity_;
    int iter = iterators_[i][j];
    while(iter < iterators_[i][j+1]){
      load-=nodes_[chromosomes_[i][iter]].demand_;
      ++iter;
    }
    if(load<0){
      iterators_[i][j+1]--;
      j--;
    }
  }

  for(int j=n_vehicles_;j>1;j--){
    int load = capacity_;
    int iter = iterators_[i][j]-1;
    while(iter >= iterators_[i][j-1]){
      load-=nodes_[chromosomes_[i][iter]].demand_;
      --iter;
    }
    if(load<0){
      iterators_[i][j-1]++;
      j++;
    }
  }
  return true;
}

bool GASolution::MutateIterRight(int i_chromosome, int j_in){
  if(j_in==n_vehicles_ || j_in==0) return false;

  int i = i_chromosome;
  if(iterators_[i][j_in] < iterators_[i][j_in-1])iterators_[i][j_in]++;
  for(int j=j_in;j>1;j--){
    int load = capacity_;
    int iter = iterators_[i][j]-1;
    while(iter >= iterators_[i][j-1]){
      load-=nodes_[chromosomes_[i][iter]].demand_;
      --iter;
    }
    if(load<0){
      iterators_[i][j-1]++;
      j++;
    }
  }

  for(int j=0;j<n_vehicles_-1;j++){
    int load = capacity_;
    int iter = iterators_[i][j];
    while(iter < iterators_[i][j+1]){
      load-=nodes_[chromosomes_[i][iter]].demand_;
      ++iter;
    }
    if(load<0){
      iterators_[i][j+1]--;
      j--;
    }
  }
  return true;
}

bool GASolution::checkValidity(int i){
  for(int j=0;j<n_vehicles_;j++){
    int load = capacity_;
    int iter = iterators_[i][j];
    while(iter < iterators_[i][j+1]){
      load-=nodes_[chromosomes_[i][iter]].demand_;
      ++iter;
    }
    if(load<0) return false;
  }
  return true;
}

void GASolution::RandomSwap(){
  int count =0;
  while(count<20){
    best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
    int r = rand()%n_chromosomes_;
    // while(r==best_) r = rand()%n_chromosomes_;
    int i1 = rand()%n_nucleotide_pairs_;
    int i2 = rand()%n_nucleotide_pairs_;
    std::swap(chromosomes_[r][i1],chromosomes_[r][i2]);
    auto temp_it = iterators_[r];
    MakeValid(r);
    double p = costs_[r];
    costs_[r] = CalculateCost(r);
    if(p<costs_[r]){
      std::swap(chromosomes_[r][i1],chromosomes_[r][i2]);
      iterators_[r] = temp_it;
      count++;
      costs_[r] = p;
    }
    else if(checkValidity(r)) break;
  }
}

void GASolution::AddBest(){
  best_ = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
  int worst = std::min_element(costs_.begin(), costs_.end()) - costs_.begin();
  chromosomes_[worst] = chromosomes_[best_];
  costs_[worst] = costs_[best_];
  iterators_[worst] = iterators_[best_];
}

void GASolution::DeleteWorstChromosome(){
  auto it = std::max_element(costs_.begin(), costs_.end());
  int i = it - costs_.begin();
  chromosomes_.erase(chromosomes_.begin() + i);
  costs_.erase(it);
  iterators_.erase(iterators_.begin() + i);
}

void GASolution::InsertIterDist(){
  int n = rand()%n_chromosomes_;
  auto temp = iterators_[n];
  int j = n_vehicles_;
  while(iterators_[n][j]==n_nucleotide_pairs_) j--;
  if(j==n_vehicles_-1) return;
  j++;
  // that found the iterator to insert
  double cost = 0;
  int iter_begin = 0;
  int range = 0;
  for(int i=0;i<n_vehicles_;i++){
    int c = 0;
    c += distanceMatrix_[0][iterators_[n][i]];
    for(int k=iterators_[n][i];k<iterators_[n][i+1]-1;k++){
      c += distanceMatrix_[chromosomes_[n][k]][chromosomes_[n][k+1]];
    }
    if(iterators_[n][i+1]-iterators_[n][i]<2) continue;
    c += distanceMatrix_[iterators_[n][i+1]-1][0];
    if(c>cost){
      cost = c;
      iter_begin = i;
      range = iterators_[n][i+1]-iterators_[n][i];
    }
  }
  int i = iter_begin;
  if(cost == 0 || range<2) return;
  int val = iterators_[n][i]+rand()%(range-1)+1;
  iterators_[n].erase(iterators_[n].begin()+j);
  iterators_[n].insert(iterators_[n].begin()+i+1, val);
  MakeValid(n); // dont think this is req
  if(!checkValidity(n)) std::cout << "Invalid from insertiterdist"<<std::endl;
  int c2 = CalculateCost(n);
  if(costs_[n]<c2) iterators_[n] = temp;
  else costs_[n] = c2;
}

void GASolution::GenerateBestSolution(){
  auto it = std::min_element(costs_.begin(), costs_.end());
  std::cout << "Cost: " << *it << std::endl;
  int i = it-costs_.begin();
  auto v = vehicles_.begin();
  for(size_t k=0;k<iterators_[0].size()-1;k++, v++){
    v->cost_ = 0;
    if(iterators_[i][k]==n_nucleotide_pairs_) break;
    int j=iterators_[i][k];
    if(j<iterators_[i][k+1]){
      v->cost_+=distanceMatrix_[0][chromosomes_[i][j]];
      v->nodes_.push_back(chromosomes_[i][j]);
      v->load_ -= nodes_[chromosomes_[i][j]].demand_;
    }
    while(j+1<iterators_[i][k+1]){
      v->cost_+=distanceMatrix_[chromosomes_[i][j]][chromosomes_[i][j+1]];
      v->nodes_.push_back(chromosomes_[i][j+1]);
      v->load_ -= nodes_[chromosomes_[i][j+1]].demand_;
      j++;
    }
    v->cost_ += distanceMatrix_[v->nodes_.back()][depot_.id_];
    v->nodes_.push_back(depot_.id_);
  }
  while(v!=vehicles_.end()){
    v->cost_ = 0;
    ++v;
  }
  std::cout << "Solution valid: " << CheckSolutionValid()<< std::endl;
}
