#include "ga_iter.hpp"

std::vector<int> GAIterSolution::GenerateRandomSolution(){
  std::vector<int> temp(n_genes);
  for(int i=0; i<n_genes; ++i){
    temp[i] = i+1;
  }
  unsigned seed = rand(); //std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(temp.begin(), temp.end(), std::default_random_engine(seed));
  return temp;
}

std::vector<int> GAIterSolution::GenerateRandomIterSolution(){
  std::vector<int> temp(n_vehicles+1);
  std::unordered_set<int> added;
  temp[0] = 0;
  added.insert(0);
  for(int i; i<n_vehicles; ++i){
    int n = rand()%(n_genes-1)+1;
    if(added.find(n)!=added.end()) n = n_genes;
    temp[i] = n;
    added.insert(n);
  }
  temp[n_vehicles] = n_genes;
  std::sort(temp.begin(), temp.end());
  // std::cout << "TEMP: ";
  // for(auto&i:temp) std::cout << i << " ";
  // std::cout << std::endl;
  return temp;
}

void GAIterSolution::GenerateRandomSolutions(){
  std::vector<int> temp(n_genes);
  for(int i=0; i<n_genes; ++i){
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
      int n = rand()%n_genes;
      if(added.find(n)!=added.end()) n = n_genes;
      temp_i[i] = n;
    }
    temp_i[n_vehicles] = n_genes;
    std::sort(temp_i.begin(), temp_i.end());
    iterators.push_back(temp_i);
  }
}

void GAIterSolution::GenerateGreedySolutions(){
  std::vector<int> gs;
  auto vehicles2 = vehicles;
  std::vector<int> iter;
  iter.push_back(0);
  // int k=0;

  for(auto& v:vehicles2){
    // k++;
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
  std::cout << "Costg: " << cost << std::endl;
  chromosomes[0] = gs;
  iterators[0] = iter;
  // std::cout << "Chromosome: ";
  // for(auto& m:chromosomes[0]) std::cout << m << " ";
  // std::cout << std::endl;
  // for(auto& m:iterators[0]) std::cout << m << " ";
  // std::cout << std::endl;
  costs[0] = NewCalculateCost(0);
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
          int i = rand()%(n_genes-1)+1;
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
    // std::cout << gs.size() << std::endl;
    // std::cout << iter.size() << std::endl;
    chromosomes[j] = gs;
    // std::cout << "Chromosome new : ";
    // for(auto& m:chromosomes[j]) std::cout << m << " ";
    // std::cout << std::endl;
    // std::cout << "Chromosome: ";
    // for(auto& m:chromosomes[j]) std::cout << m << " ";
    // std::cout << std::endl;
    iterators[j] = iter;
    // for(auto& m:iterators[j]) std::cout << m << " ";
    // std::cout << std::endl;
    MakeValid(j);
    // for(auto& m:iterators[j]) std::cout << m << " ";
    // std::cout << std::endl;
    if(!checkValidity(j)) std::cout << "Oi"<<std::endl;
    // std::cout << "done with make valid" << std::endl;
    costs[j] = NewCalculateCost(j);
    // for(auto& m:iterators[j]) std::cout << m << " ";
    // std::cout << std::endl;
    // std::cout << "done with cal" << std::endl;
  }
  // vehicles = vehicles2;
  // exit(0);
}

void GAIterSolution::RemoveSimilarSolutions(){
  std::set<int> to_delete;
  for(int i=0;i<n_chromosomes;i++){
    for(int j=i+1;j<n_chromosomes;j++){
      if(j==best) continue;
      int count = 0;
      for(int k=0;k<n_genes;k++){
        if(chromosomes[i][k]==chromosomes[j][k]) count++;
      }
      if(count > 0.95*n_genes){
        to_delete.insert(j);
      }
    }
  }
  for(auto it = to_delete.begin();it!=to_delete.end(); ++it){
    if(rand()%100 > 15){
      chromosomes[*it] = GenerateRandomSolution();
      iterators[*it] = GenerateRandomIterSolution();
      MakeValid(*it);
      costs[*it] = NewCalculateCost(*it);
    }
  }
}

double GAIterSolution::NewCalculateCost(int i){
  double cost = 0;
  double tmp = costs[i];
  //std::cout << "-1-" << std::endl;

  for(int k=0;k<iterators[0].size()-1;k++){
    //std::cout << "-2-" << std::endl;

    if(iterators[i][k]==n_genes) break;
    //std::cout << "-3-" << std::endl;

    int j=iterators[i][k];
    if(j<iterators[i][k+1]) cost+=distanceMatrix[0][chromosomes[i][j]];
    while(j+1<iterators[i][k+1]){
      cost+=distanceMatrix[chromosomes[i][j]][chromosomes[i][j+1]];
      j++;
    }
    //std::cout << "-4-" << std::endl;
    // std::cout << i << " " << j << std::endl;
    cost+=distanceMatrix[chromosomes[i][j]][0];
    //std::cout << "-4.1-" << std::endl;

  }
  //std::cout << "-5-" << std::endl;
  return cost;
}

void GAIterSolution::CalculateTotalCost(){
  for(int i=0;i<n_chromosomes;i++){
    double tmp = costs[i];
    costs[i] = NewCalculateCost(i);
    if(costs[i]>tmp){

    }
  }
}

void GAIterSolution::Solve(){
  GenerateRandomSolutions();
  // std::cout << std::endl << std::endl;
  // std::cout << "AFTER RANDOM"<< std::endl;
  // for(auto p:iterators){
  //   for(auto& m:p) std::cout << m << " ";
  //   std::cout << std::endl;
  // }
  for(int i=0;i<n_chromosomes;i++) MakeValid(i);
  // std::cout << std::endl << std::endl;
  // std::cout << "AFTER MV "<< std::endl;
  // for(auto p:iterators){
  //   for(auto& m:p) std::cout << m << " ";
  //   std::cout << std::endl;
  // }
  GenerateGreedySolutions();
  CalculateTotalCost();
  best = std::min_element(costs.begin(), costs.end()) - costs.begin();
  int generation = 0;
  while(generation < generations){
    // std::cout << std::endl << std::endl;
    // std::cout << "IN LOOP"<< std::endl;
    // for(auto p:iterators){
    //   for(auto& m:p) std::cout << m << " ";
    //   std::cout << std::endl;
    // }
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    if(rand()%2==0){
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      auto best1 = best;
      NAEXCrossover();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      // if(best<best1) std::cout << "NAEX made a diff" << std::endl;
    }
    else if(rand()%2==0){
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      auto best1 = best;
      // AEXCrossover();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      // if(best<best1) std::cout << "AEX made a diff" << std::endl;
    }
    if(rand()%100<100){
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      auto best1 = best;
      int n = rand()%n_chromosomes;
      while(n==best) n = rand()%n_chromosomes;
      MutateIterLeft(n, rand()%n_vehicles);
      costs[n] = NewCalculateCost(n);
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      if(best<best1) std::cout << "MIL made a diff" << std::endl;
      // continue;
    }
    else if(rand()%100<50){
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      auto best1 = best;

      int n = rand()%n_chromosomes;
      auto temp_i = iterators[n];
      MutateIterRight(n, rand()%n_vehicles);
      costs[n] = NewCalculateCost(n);
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();

      if(best<best1) std::cout << "MIR made a diff" << std::endl;
      else iterators[n] = temp_i;
      costs[n] = NewCalculateCost(n);

      // continue;
    }
    // if(rand()%100<30){
    //   best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    //
    //   auto best1 = std::min_element(costs.begin(), costs.end()) - costs.begin();
    //   Mutate();
    //   best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    //
    //   auto best2 = std::min_element(costs.begin(), costs.end()) - costs.begin();
    //   if(best1 > best2) std::cout << "Mutate made a diff" << std::endl;
    // }
    if(rand()%100<30) {
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      auto best1 = best;
      RandomSwap();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      // if(best<best1) std::cout << "RandomSwap made a diff" << std::endl;
    }
    if(rand()%100<30) {
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      auto best1 = best;

      MutateWhithinAlele();

      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      // if(best<best1) std::cout << "MutateWhithinAlele made a diff" << std::endl;
    }
    if(rand()%100<30) {
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      auto best1 = best;

      RandomSwapAlele();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      // if(best<best1) std::cout << "RandomSwapAlele made a diff" << std::endl;
    }
    if(rand()%100<75) {
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      auto best1 = best;

      InsertIterDist();
      best = std::min_element(costs.begin(), costs.end()) - costs.begin();
      if(best<best1) std::cout << "InsertIterDist made a diff" << std::endl;
    }
    // if(rand()%100<5) {
    //   best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    //   AddBest();
    //   best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    // }
    // if(rand()%100<20) {
    //   DeleteBadChromosome();
    // }
    CalculateTotalCost();
    generation++;
    if(generation%100==0){
      // RemoveSimilarSolutions();
    }
    // for(int m=0;m<n_chromosomes;m++){
    //   for(auto&i:chromosomes[m]) std::cout << i << " ";
    //   std::cout << std::endl;
    //   // for(auto&i:iterators[m]) std::cout << i << " ";
    //   // std::cout << std::endl;
    // }
    // std::cout << std::endl;

  }
  // for(int i=0; i< n_chromosomes;i++){
  //   for(auto& j:chromosomes[i]){
  //     std::cout << std::setw(3) <<j ;
  //   }
  //   std::cout << " | " << costs[i] << std::endl;
  // }
  GenerateBestSolution();
  double cost = 0;
  for(auto &v:vehicles) cost +=v.cost;
  // std::cout << "Final cost: " << cost << std::endl;
}

void GAIterSolution::AEXCrossover(){
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
  iterators.emplace_back(GenerateRandomIterSolution());
  MakeValid(n_chromosomes);
  if(checkValidity(n_chromosomes)){
    costs.emplace_back(NewCalculateCost(n_chromosomes));
    InsertionBySimilarity();
  }
  else{
    iterators.erase(iterators.begin()+n_chromosomes);
    chromosomes.erase(chromosomes.begin()+n_chromosomes);
  }
}

void GAIterSolution::NAEXCrossover(){
  // std::cout << "1" << std::endl;

  int p1 = TournamentSelection();
  int p2 = TournamentSelection();
  std::vector<int> child;
  std::unordered_set<int> reached;
  auto itp1 = &(chromosomes[p1]);
  auto itp2 = &(chromosomes[p2]);
  child.push_back((*itp1)[0]);
  reached.insert(child.back());
  int n1, n2;
  // std::cout << "2" << std::endl;

  while(child.size() < n_genes){
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
    // std::cout << "3" << std::endl;

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
    // std::cout << "4" << std::endl;
    // std::cout << child.back() << " " << n1 << std::endl;
    if(distanceMatrix[child.back()][n1] > distanceMatrix[child.back()][n2]) std::swap(n1,n2);
    // std::cout << "4.2" << std::endl;

    child.push_back(n1);
    // std::cout << "4.3" << std::endl;

    reached.insert(n1);
    // std::cout << "4.4" << std::endl;

  }
  // std::cout << "5" << std::endl;
  // for(auto m:child) std::cout << m << " ";
  // std::cout << std::endl;
  chromosomes.push_back(child);
  // iterators.emplace_back(GenerateRandomIterSolution());
  iterators.emplace_back(iterators[p1]);
  MakeValid(n_chromosomes);
  if(checkValidity(n_chromosomes)){
    costs.emplace_back(NewCalculateCost(n_chromosomes));
    InsertionBySimilarity();
  }
  else{
    iterators.erase(iterators.begin()+n_chromosomes);
    chromosomes.erase(chromosomes.begin()+n_chromosomes);
  }
  // std::cout << "6" << std::endl;
}

void GAIterSolution::MakeValid(int i){
    for(int j=0;j<n_vehicles-1;j++){
      int load = vehicle_capacity;
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
      int load = vehicle_capacity;
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

void GAIterSolution::DeleteBadChromosome(){
  // int i = TournamentSelectionBad();
  // chromosomes[i] = GenerateRandomSolution();
}

int GAIterSolution::TournamentSelection(){
  int i1 = rand()%chromosomes.size();
  int i2 = rand()%chromosomes.size();
  int i3 = rand()%chromosomes.size();
  if(costs[i1] <= costs[i2] && costs[i1] <= costs[i3]) return i1;
  if(costs[i2] <= costs[i1] && costs[i2] <= costs[i3]) return i2;
  if(costs[i3] <= costs[i1] && costs[i3] <= costs[i2]) return i3;
}

int GAIterSolution::TournamentSelectionBad(){
  int i1 = rand()%chromosomes.size();
  int i2 = rand()%chromosomes.size();
  int i3 = rand()%chromosomes.size();
  if(costs[i1] >= costs[i2] && costs[i1] >= costs[i3]) return i1;
  if(costs[i2] >= costs[i1] && costs[i2] >= costs[i3]) return i2;
  if(costs[i3] >= costs[i1] && costs[i3] >= costs[i2]) return i3;
}

void GAIterSolution::InsertionBySimilarity(){
  best = std::min_element(costs.begin(), costs.end()) - costs.begin();
  bool flag = true;
  for(int i=0;i<n_genes;++i){
    if(i!=best && abs(costs.back()-costs[i]) < 2*(costs[best]/100.0)){
      costs.erase(costs.begin()+i);
      chromosomes.erase(chromosomes.begin()+i);
      iterators.erase(iterators.begin()+i);
      flag = false;
      break;
    }
  }
  if(flag) DeleteWorstChromosome();
}

void GAIterSolution::Mutate(){
  int count =0;
  while(count<20){
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    // while(r==best) r = rand()%n_chromosomes;
    int i1 = rand()%n_genes;
    int i2 = rand()%n_genes;
    if(i1>i2) std::swap(i1,i2);
    auto temp_it = iterators[r];
    std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
    MakeValid(r);
    double p = costs[r];
    costs[r] = NewCalculateCost(r);
    if(p<costs[r]){
      std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
      iterators[r] = temp_it;
      count++;
      costs[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

void GAIterSolution::SwapWhithinAlele(){
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
    costs[r] = NewCalculateCost(r);
    if(p<costs[r]){
      std::swap(chromosomes[r][i1], chromosomes[r][i2]);
      iterators[r] = temp_it;
      count++;
      costs[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

void GAIterSolution::MutateWhithinAlele(){
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
    costs[r] = NewCalculateCost(r);
    if(p<costs[r]){
      std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
      iterators[r] = temp_it;
      count++;
      costs[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

bool GAIterSolution::MutateIterLeft(int i_chromosome, int j_in){
  if(j_in==n_vehicles || j_in==0) return false;

  int i = i_chromosome;
  auto temp = iterators[i][j_in];

  if(iterators[i][j_in] > iterators[i][j_in-1])iterators[i][j_in]--;
  for(int j=j_in;j<n_vehicles-1;j++){
    int load = vehicle_capacity;
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
    int load = vehicle_capacity;
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

bool GAIterSolution::MutateIterRight(int i_chromosome, int j_in){
  if(j_in==n_vehicles || j_in==0) return false;

  int i = i_chromosome;
  auto temp = iterators[i][j_in];

  if(iterators[i][j_in] < iterators[i][j_in-1])iterators[i][j_in]++;

  for(int j=j_in;j>1;j--){
    int load = vehicle_capacity;
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
    int load = vehicle_capacity;
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

bool GAIterSolution::checkValidity(int i){
  for(int j=0;j<n_vehicles;j++){
    int load = vehicle_capacity;
    int iter = iterators[i][j];
    while(iter < iterators[i][j+1]){
      load-=nodes[chromosomes[i][iter]].demand;
      ++iter;
    }
    if(load<0) return false;
  }
  return true;
}

void GAIterSolution::RandomSwap(){
  int count =0;
  while(count<20){
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    // while(r==best) r = rand()%n_chromosomes;
    int i1 = rand()%n_genes;
    int i2 = rand()%n_genes;
    std::swap(chromosomes[r][i1],chromosomes[r][i2]);
    auto temp_it = iterators[r];
    double p = costs[r];
    costs[r] = NewCalculateCost(r);
    if(p<costs[r]){
      std::swap(chromosomes[r][i1],chromosomes[r][i2]);
      iterators[r] = temp_it;
      count++;
      costs[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

void GAIterSolution::AddBest(){
  best = std::min_element(costs.begin(), costs.end()) - costs.begin();
  int worst = std::min_element(costs.begin(), costs.end()) - costs.begin();
  chromosomes[worst] = chromosomes[best];
  costs[worst] = costs[best];
  iterators[worst] = iterators[best];
}

void GAIterSolution::RandomSwapAlele(){
  int count =0;
  while(count<20){
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    // while(r==best) r = rand()%n_chromosomes;
    int i1 = rand()%n_genes;
    int i2 = rand()%n_genes;
    std::reverse(chromosomes[r].begin() + i1,chromosomes[r].begin() + i2);
    auto temp_it = iterators[r];
    MakeValid(r);
    double p = costs[r];
    costs[r] = NewCalculateCost(r);
    if(p<costs[r]){
      std::reverse(chromosomes[r].begin() + i1,chromosomes[r].begin() + i2);
      iterators[r] = temp_it;
      count++;
      costs[r] = p;
    }
    else if( checkValidity(r)) break;
  }
}

void GAIterSolution::DeleteWorstChromosome(){
  auto it = std::max_element(costs.begin(), costs.end());
  int i = it - costs.begin();
  chromosomes.erase(chromosomes.begin() + i);
  costs.erase(it);
  iterators.erase(iterators.begin() + i);
}

void GAIterSolution::InsertIterDist(){
  // std::cout << "1"<<std::endl;
  int n = rand()%n_chromosomes;
  auto temp = iterators[n];
  int j = n_vehicles;
  while(iterators[n][j]==n_genes) j--;
  if(j==n_vehicles-1) return;
  j++;
  //that found the ierator to insert
  // std::cout << "2"<<std::endl;
  // for(auto& m:iterators[n]) std::cout << m <<" ";
  // std::cout << std::endl;
  double cost = 0;
  int iter_begin;
  int range = 0;
  for(int i=0;i<n_vehicles;i++){
    int c = 0;
    c += distanceMatrix[0][iterators[n][i]];
    for(int k=iterators[n][i];k<iterators[n][i+1]-1;k++){
      c += distanceMatrix[chromosomes[n][k]][chromosomes[n][k+1]];
    }
    // std::cout << "3"<<std::endl;
    // std::cout << "--- : iterators[n][i] " << iterators[n][i]<<std::endl;
    // for(auto& m:iterators[n]) std::cout << m <<" ";
    // std::cout << std::endl;
    // std::cout << "--- : iterators[n][i+1] " << iterators[n][i+1] << std::endl;
    if(iterators[n][i+1]-iterators[n][i]<2) continue;
    c += distanceMatrix[iterators[n][i+1]-1][0];
    if(c>cost){
      cost = c;
      iter_begin = i;
      range = iterators[n][i+1]-iterators[n][i];
    }
  }
  int i = iter_begin;
  // std::cout << "4"<<std::endl;
  if(cost = 0 || range<2) return;
  // std::cout << "iterators[n][i]"<<iterators[n][i]<<std::endl;

  int val = iterators[n][i]+rand()%(range-1)+1;
  // std::cout << "Val: " << val <<std::endl;
  // std::cout << "-----------------"<<std::endl;
  // for(auto& m:iterators[n]) std::cout << m <<" ";
  // std::cout << std::endl;
  iterators[n].erase(iterators[n].begin()+j);
  iterators[n].insert(iterators[n].begin()+i+1, val);
  // for(auto& m:iterators[n]) std::cout << m <<" ";
  // std::cout << std::endl;

  MakeValid(n); // dont  htink this is req
  // std::cout << "5"<<std::endl;

  int c2 = NewCalculateCost(n);
  if(costs[n]<c2) iterators[n] = temp;
  else costs[n] = c2;
}

void GAIterSolution::GenerateBestSolution(){
  auto it = std::min_element(costs.begin(), costs.end());
  std::cout << "Cost: " << *it << std::endl;
  int i = it-costs.begin();
  for(auto& m:chromosomes[i])std::cout << m << " ";
  std::cout << std::endl;
  for(auto& m:iterators[i])std::cout << m << " ";
  std::cout << std::endl;

  // std::cout << "Valid" << checkValidity(i) << std::endl;
  auto v = vehicles.begin();
  for(int k=0;k<iterators[0].size()-1;k++, v++){
    v->cost = 0;
    if(iterators[i][k]==n_genes) break;
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
  for(auto& v2:vehicles) v2.PrintRoute();
}

// int main(){
//
//   Problem p;
//   GAIterSolution g(p.nodes, p.vehicles, p.distanceMatrix, 30, 100000);
//   g.Solve();
//   // g.GenerateRandomSolutions();
//   // g.GenerateGreedySolutions();
//   // g.MutateIterLeft(0,0);
//   // g.MutateIterLeft(0,1000);
//   return 0;
// }
