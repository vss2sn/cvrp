#include "ga_iter.hpp"

// Still need to account for case if nodes cannot be put into vehilces due to small number of vehicles in initial solution

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
  for(int i=0; i<n_vehicles-1; ++i){
    int n = rand()%n_genes;
    if(added.find(n)!=added.end()) n = n_genes;
    temp[i] = n;
  }
  temp[n_vehicles] = n_genes;
  std::sort(temp.begin(), temp.end());
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
  //std::cout << "-------------------------------" << std::endl;
  for(int j=0;j<n_chromosomes;j++){
    std::vector<int> temp_i(n_vehicles+1);
    std::unordered_set<int> added;
    for(int i=0; i<n_vehicles; ++i){
      int n = rand()%n_genes;
      if(added.find(n)!=added.end()) n = n_genes;
      temp_i[i] = n;
    }
    temp_i[n_vehicles] = n_genes;
    std::sort(temp_i.begin(), temp_i.end());
    // for(auto iter:temp_i) std::cout << iter << " ";
    // std::cout << std::endl;
    iterators.push_back(temp_i);
  }
  //std::cout << "-------------------------------" << std::endl;
}

void GAIterSolution::GenerateGreedySolutions(){
  std::vector<int> gs;
  auto vehicles2 = vehicles;
  std::vector<int> iter;
  iter.push_back(0);
  for(auto& v:vehicles2){
    while(true){
      Node closest_node = find_closest(v, distanceMatrix, nodes);
      if(closest_node.id!=-1 && v.load - closest_node.demand >=0){
        // std::cout << closest_node.id << std::endl;
        v.load -= closest_node.demand;
        v.cost += distanceMatrix[v.nodes.back()][closest_node.id];
        // std::cout << v.nodes.back() << " " << closest_node.id << " " << distanceMatrix[v.nodes.back()][closest_node.id] << std::endl;
        v.nodes.push_back(closest_node.id);
        gs.push_back(closest_node.id);
        nodes[closest_node.id].is_routed = true;
      }
      else{
        iter.push_back(iter.back() + v.nodes.size()-1);
        // std::cout << "ITER route ends with " << gs[iter.back()-1] << std::endl;
        v.cost += distanceMatrix[v.nodes.back()][depot.id];
        // std::cout << v.nodes.back() << " " << depot.id << " " << distanceMatrix[v.nodes.back()][depot.id] << std::endl;
        v.nodes.push_back(depot.id);
        break;
      }
    }
  }
  // std::cout << "Last element in iter: " << iter.back() << std::endl;
  // for(auto&i:iter) std::cout << i << " ";
  // std::cout << std::endl;
  double cost = 0;
  for(auto& v:vehicles2) cost += v.cost;
  std::cout << "Costg: " << cost << std::endl;
  chromosomes[0] = gs;
  iterators[0] = iter;
  costs[0] = NewCalculateCost(0);
  // std::cout << "Costg2: " << costs[0] << std::endl;
  // std::cout << "Is greedy valid:" << checkValidity(0) << std::endl;
  for(int j=0;j<n_chromosomes;++j){
    // int i = rand()%n_chromosomes;
    chromosomes[j] = gs;
    iterators[j] = iter;
    costs[j] = NewCalculateCost(j);
    // std::cout << "In greedy addition loop. cost: " << costs[i] << std::endl;
  }
  // std::cout << costs[i] << std::endl;

  // for(auto&g:gs) std::cout << g << " " ;
  // std::cout << std::endl;
  // std::cout << "OI:!!!!" <<std::endl << NewCalculateCost(i)<<std::endl;
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
      iterators[*it] = GenerateRandomIterSolution();
      costs[*it] = NewCalculateCost(*it);
    }

  }
}


double GAIterSolution::NewCalculateCost(int i){
  double cost = 0;
  double tmp = costs[i];
  // std::cout << "i: " << i << " " << iterators.size() << std::endl;
  for(int k=0;k<iterators[0].size()-1;k++){
    // std::cout << "k: " << k << std::endl;
    if(iterators[i][k]==n_genes) break;
    int j=iterators[i][k];
    if(j<iterators[i][k+1]) cost+=distanceMatrix[0][chromosomes[i][j]];
    while(j+1<iterators[i][k+1]){
      // std::cout << "j: " << j << " " << iterators[i][k+1] << std::endl;
      cost+=distanceMatrix[chromosomes[i][j]][chromosomes[i][j+1]];
      // std::cout << "Calculated cost " << std::endl;
      j++;
    }
    cost+=distanceMatrix[chromosomes[i][j]][0];
  }
  if(tmp<cost){
    //std::cout << "---------------------------12------------------------------------  " << tmp << "--------------  " << cost << std::endl;

  }

  // std::cout << "Returning" << std::endl;
  return cost;
}

// double GAIterSolution::CalculateCost(int i){
//   int j=0;
//   double cost = 0;
//   while(j<n_genes){
//     // std::cout << "In whle " << j<<" " << nodes[chromosomes[i][j]].demand<< std::endl;
//     // std::cout << chromosomes.size() << " " << chromosomes[0].size() << std::endl;
//     // std::cout << "Vehicle capacity: " << vehicle_capacity << std::endl;
//     int load = vehicle_capacity - nodes[chromosomes[i][j]].demand;
//     // std::cout << "0 " << chromosomes[i][j]  << std::endl;
//
//     // std::cout << "Load: " << load << std::endl;
//     cost += distanceMatrix[0][chromosomes[i][j]];
//     while(j+1 < n_genes && load - nodes[chromosomes[i][j+1]].demand >=0){
//       //std::cout << "------------" << std::endl;
//       // std::cout << nodes[chromosomes[i][j+1]].demand << std::endl;
//       load -=nodes[chromosomes[i][j+1]].demand;
//       // std::cout << "Load: " << load << std::endl;
//       // std::cout << "2 " << chromosomes[i][j] << " " <<chromosomes[i][j+1]<<std::endl;
//       cost += distanceMatrix[chromosomes[i][j]][chromosomes[i][j+1]];
//       // std::cout << chromosomes[i][j] <<" " <<chromosomes[i][j+1] << std::endl;
//       j++;
//       // std::cout << "3" << std::endl;
//     }
//     // std::cout << "out" << std::endl;
//     cost += distanceMatrix[chromosomes[i][j]][0];
//     // std::cout << chromosomes[i][j] <<" " << "0" << std::endl;
//     j++;
//   }
//   // std::cout << "Returning" << std::endl;
//   // exit(0);
//   return cost;
// }

void GAIterSolution::CalculateTotalCost(){
  for(int i=0;i<n_chromosomes;i++){
    double tmp = costs[i];
    costs[i] = NewCalculateCost(i);
    if(costs[i]>tmp){
      //std::cout << "---------------------------11------------------------------------" << std::endl;

    }
  }
}

void GAIterSolution::Solve(){
  GenerateRandomSolutions();
  GenerateGreedySolutions();
  CalculateTotalCost();
  best = std::min_element(costs.begin(), costs.end()) - costs.begin();
  // std::cout << "Best init cost: " << costs[best] << std::endl;
  int generation = 0;
  while(generation < generations){
    // std::cout << "Generation: " << generation << std::endl;
    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    if(rand()%2==0){
      // std::cout << "-----";
      NAEXCrossover();
      // for(auto i:chromosomes[n_chromosomes-1]) std::cout << i << " ";
      // std::cout << std::endl;
      // for(auto i:iterators[n_chromosomes-1]) std::cout << i << " ";
      // std::cout << std::endl;
      // MutateIterLeft(n_chromosomes-1, 1);
      // for(auto i:chromosomes[n_chromosomes-1]) std::cout << i << " ";
      // std::cout << std::endl;
      // for(auto i:iterators[n_chromosomes-1]) std::cout << i << " ";
      // std::cout << std::endl;
      // MutateIterRight(n_chromosomes-1, n_vehicles - 1);
      // for(auto i:chromosomes[n_chromosomes-1]) std::cout << i << " ";
      // std::cout << std::endl;
      // for(auto i:iterators[n_chromosomes-1]) std::cout << i << " ";
      // std::cout << std::endl;
      // std::cout << "Is naex valid: " << checkValidity(n_chromosomes-1) << std::endl;
      // std::cout << "-----";
       //std::cout << "---------------------------1------------------------------------" << std::endl;
    }
    // else if(rand()%2==0){
    //   AEXCrossover();
    //   MutateIterLeft(n_chromosomes-1, 1);
    //   MutateIterLeft(n_chromosomes-1, n_vehicles - 1);
    //    //std::cout << "---------------------------2------------------------------------" << std::endl;
    //
    // }
    // if(rand()%100<20){
    //   Mutate();
    //    //std::cout << "---------------------------3------------------------------------" << std::endl;
    // }
    if(rand()%100<30){
      // std::cout << "Mutating left"<< std::endl;
      int n = rand()%n_chromosomes;
      while(n==best) n = rand()%n_chromosomes;
      MutateIterLeft(n, rand()%n_vehicles);
       //std::cout << "---------------------------3.1------------------------------------" << std::endl;

    }
    if(rand()%100<30){
      // std::cout << "Mutating right"<< std::endl;
      int n = rand()%n_chromosomes;
      while(n==best) n = rand()%n_chromosomes;
      MutateIterRight(n, rand()%n_vehicles);
       //std::cout << "---------------------------3.1------------------------------------" << std::endl;

    }
    // if(rand()%100<20) {
    //   RandomSwap();
    //    //std::cout << "---------------------------4------------------------------------" << std::endl;
    //
    // }
    // if(rand()%100<20) {
    //   DeleteBadChromosome();
    //    //std::cout << "---------------------------5------------------------------------" << std::endl;
    //
    // }
    CalculateTotalCost();
     //std::cout << "---------------------------10------------------------------------" << std::endl;

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
       //std::cout << "---------------------------6------------------------------------" << std::endl;
      // for(int i=0; i< n_chromosomes;i++){
      //   for(auto& j:chromosomes[i]){
      //     std::cout << std::setw(3) <<j ;
      //   }
      //   std::cout << " | " << costs[i] << std::endl;
      // }
    }
    // for(int m=0;m<n_chromosomes;m++){
    //   for(auto&i:chromosomes[m]) std::cout << i << " ";
    //   std::cout << std::endl;
    //   for(auto&i:iterators[m]) std::cout << i << " ";
    //   std::cout << std::endl;
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
  // for(auto& v:vehicles) v.PrintRoute();
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
  iterators.push_back(GenerateRandomIterSolution());
  costs.emplace_back(NewCalculateCost(n_chromosomes));
  // DeleteWorstChromosome();
  // std::cout << "AEX"<<std::endl;
  InsertionBySimilarity();
}

void GAIterSolution::NAEXCrossover(){
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
    // std::cout << n1 << " " << n2 << std::endl;
    if(distanceMatrix[child.back()][n1] > distanceMatrix[child.back()][n2]) std::swap(n1,n2);
    child.push_back(n1);
    reached.insert(n1);
  }
  // std::cout << "Out while" << std::endl;
  chromosomes.push_back(child);
  iterators.emplace_back(GenerateRandomIterSolution());
  MakeValid(n_chromosomes);
  // MutateIterLeft(n_chromosomes, 1);
  // MutateIterRight(n_chromosomes, n_vehicles-1);
  if(checkValidity(n_chromosomes)){
    // std::cout << "Valid -------------------------------------------------------" << std::endl;
    // for(auto& i:iterators[n_chromosomes]) std::cout << i << std::endl;
    // std::cout << std::endl;
    costs.emplace_back(NewCalculateCost(n_chromosomes));
    // std::cout << "Valid -------------------------------------------------------//" << std::endl;

    // DeleteWorstChromosome();
    // std::cout << "AEX"<<std::endl;
    InsertionBySimilarity();
  }
  else{
    std::cout << "not Valid " << std::endl;
    for(auto& i:iterators[n_chromosomes]) std::cout << i << " ";
    std::cout << std::endl;
    iterators.erase(iterators.begin()+n_chromosomes);
    chromosomes.erase(chromosomes.begin()+n_chromosomes);

  }

  // chromosomes.push_back(child);
  // iterators.emplace_back(iterators[p1]);
  // costs.emplace_back(NewCalculateCost(n_chromosomes));
  // InsertionBySimilarity();
  //
  // chromosomes.push_back(child);
  // iterators.emplace_back(iterators[p2]);
  // costs.emplace_back(NewCalculateCost(n_chromosomes));
  // InsertionBySimilarity();
  // std::cout << "Out function" << std::endl;
}

void GAIterSolution::MakeValid(int i){
  // std::cout << "In mv" << std::endl;
  if(rand()%2==0){
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
  else{
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
  }
  // std::cout << "out mv" << std::endl;
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
   //std::cout << "---------------------------7------------------------------------" << std::endl;

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
  while(count<5){
     //std::cout << "---------------------------8------------------------------------" << std::endl;

    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    while(r==best) r = rand()%n_chromosomes;
    int i1 = rand()%n_genes;
    int i2 = rand()%n_genes;
    if(i1>i2) std::swap(i1,i2);
    std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
    double p = costs[r];
    std::reverse(chromosomes[r].begin() + i1, chromosomes[r].begin() + i2);
    costs[r] = NewCalculateCost(r);
    if(p<costs[r]){
      count++;
      costs[r] = p;
    }
    else break;
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
  // if(iterators[i][j_in]!=temp) std::cout << "Mutated Iter Left Valid: " << checkValidity(i) << std::endl;
  // Still some cases being missed
  return true;
  // std::cout << iterators.size() << std::endl;
  // std::cout << iterators[0].size() << std::endl;
  // std::cout << "Moving " << j << "th point in iterator whose value is: " << std::endl;
  // std::cout << iterators[i_chromosome][j] << std::endl;
  // std::cout << "Original Iter: ";
  // for(auto&i:iterators[i_chromosome]) std::cout << i << " ";
  // std::cout << std::endl;
  // if(j==n_vehicles || j==0) return false;
  // // std::cout << "n_genes: " << n_genes << std::endl;
  // if(iterators[i_chromosome][j-1]!=iterators[i_chromosome][j]){//check if sequential
  //   iterators[i_chromosome][j]--;
  //
  //   int load = vehicle_capacity;
  //   // for(int i=iterators[i_chromosome][j-1]; i<iterators[i_chromosome][j]; i++){ // should this be fro j instead of j-1?
  //   //     load -= nodes[chromosomes[i_chromosome][i]].demand;
  //   // }
  //   // if(load<0 && !MutateIterLeft(i_chromosome, j)){
  //   //   iterators[i_chromosome][j]++;
  //   //   return false;
  //   // }
  //   //
  //   // load = vehicle_capacity;
  //   for(int i=iterators[i_chromosome][j]; i<iterators[i_chromosome][j+1]; i++){ // should this be fro j instead of j-1?
  //       load -= nodes[chromosomes[i_chromosome][i]].demand;
  //   }
  //   if(load>=0){
  //     // std::cout << "New Iter     : ";
  //     // for(auto&i:iterators[i_chromosome]) std::cout << i << " ";
  //     // std::cout << std::endl;
  //     return true;
  //   }
  //   else{
  //     // std::cout << "Recursing" << std::endl;
  //     if(!MutateIterLeft(i_chromosome, j+1)){
  //       // std::cout << "Unchanged" << std::endl;
  //       iterators[i_chromosome][j]++;
  //       return false;
  //     }
  //     else return true;
  //   }
  // }
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


  // if(iterators[i][j_in]!=temp) std::cout << "Mutated Iter Right Valid: " << checkValidity(i) << std::endl;
  // Still some cases being missed
  return true;
  // std::cout << iterators.size() << std::endl;
  // std::cout << iterators[0].size() << std::endl;
  // std::cout << "Moving " << j << "th point in iterator whose value is: " << std::endl;
  // std::cout << iterators[i_chromosome][j] << std::endl;
  // std::cout << "Original Iter: ";
  // for(auto&i:iterators[i_chromosome]) std::cout << i << " ";
  // std::cout << std::endl;
  // if(j==n_vehicles || j==0) return false;
  // // std::cout << "n_genes: " << n_genes << std::endl;
  // if(iterators[i_chromosome][j+1]!=iterators[i_chromosome][j]){//check if sequential
  //   iterators[i_chromosome][j]++;
  //   int load = vehicle_capacity;
  //
  //   // for(int i=iterators[i_chromosome][j]; i<iterators[i_chromosome][j++]; i++){ // should this be fro j instead of j-1?
  //   //     load -= nodes[chromosomes[i_chromosome][i]].demand;
  //   // }
  //   // if(load<0 && !MutateIterRight(i_chromosome, j)){
  //   //   iterators[i_chromosome][j]--;
  //   //   return false;
  //   // }
  //   //
  //   // load = vehicle_capacity;
  //   for(int i=iterators[i_chromosome][j-1]; i<iterators[i_chromosome][j]; i++){
  //       load -= nodes[chromosomes[i_chromosome][i]].demand;
  //   }
  //   if(load>=0){
  //     // std::cout << "New Iter     : ";
  //     // for(auto&i:iterators[i_chromosome]) std::cout << i << " ";
  //     // std::cout << std::endl;
  //     return true;
  //   }
  //   else{
  //     // std::cout << "Recursing" << std::endl;
  //     if(!MutateIterRight(i_chromosome, j-1)){
  //       // std::cout << "Unchanged" << std::endl;
  //       iterators[i_chromosome][j]--;
  //       return false;
  //     }
  //     else return true;
  //   }
  // }
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
  while(count<5){
    //  std::cout << "OI!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1" << std::endl;
     //std::cout << "---------------------------9------------------------------------" << std::endl;

    best = std::min_element(costs.begin(), costs.end()) - costs.begin();
    int r = rand()%n_chromosomes;
    while(r==best) r = rand()%n_chromosomes;
    int i1 = rand()%n_genes;
    int i2 = rand()%n_genes;
    std::swap(chromosomes[r][i1],chromosomes[r][i2]);
    double p = costs[r];
    costs[r] = NewCalculateCost(r);
    if(p<costs[r]){
      std::swap(chromosomes[r][i1],chromosomes[r][i2]);
      count++;
      costs[r] = p;
    }
    else break;
  }
}

void GAIterSolution::DeleteWorstChromosome(){
  auto it = std::max_element(costs.begin(), costs.end());
  int i = it - costs.begin();
  chromosomes.erase(chromosomes.begin() + i);
  costs.erase(it);
  iterators.erase(iterators.begin() + i);
}

void GAIterSolution::GenerateBestSolution(){
  auto it = std::min_element(costs.begin(), costs.end());
  std::cout << "Cost: " << *it << std::endl;
  int i = it-costs.begin();
  std::cout << "Valid" << checkValidity(i) << std::endl;
  // int i = it - costs.begin();
  // std::cout << "Calculaed cost: " << NewCalculateCost(i) << std::endl;
  // for(auto j:chromosomes[i]) std::cout << std::setw(3) << j;
  // std::cout << std::endl;
  // int j=0;
  // for(auto& v:vehicles){
  //   // v.nodes.push_back(0);
  //   // std::cout << "About to enter of while" << std::endl;
  //   while(j < n_genes && v.load - nodes[chromosomes[i][j]].demand >= 0){
  //     // std::cout<< "In while" << std::endl;
  //     v.cost += distanceMatrix[v.nodes.back()][nodes[chromosomes[i][j]].id];
  //     v.nodes.push_back(nodes[chromosomes[i][j]].id);
  //     v.load -= nodes[chromosomes[i][j]].demand;
  //     j++;
  //   }
  //   // std::cout << "Out of while" << std::endl;
  //   v.cost += distanceMatrix[v.nodes.back()][0];
  //   v.nodes.push_back(0);
  // }
  // std::sort(chromosomes[i].begin(), chromosomes[i].end());
  // for(auto j:chromosomes[i]) std::cout << std::setw(3) << j;


  // std::cout << "i: " << i << " " << iterators.size() << std::endl;
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
      // std::cout << "j: " << j << " " << iterators[i][k+1] << std::endl;
      v->cost+=distanceMatrix[chromosomes[i][j]][chromosomes[i][j+1]];
      v->nodes.push_back(chromosomes[i][j+1]);
      v->load -= nodes[chromosomes[i][j+1]].demand;
      // std::cout << "Calculated cost " << std::endl;
      j++;
    }
    v->cost += distanceMatrix[v->nodes.back()][0];
    v->nodes.push_back(0);
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
