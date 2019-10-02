#include "main.hpp"

int main(){
  Problem p(100,4,25,16,10, "cluster");

  GreedySolution vrp_greedy(p);
  vrp_greedy.Solve();

  LocalSearchIntraSolution vrp_lsi(p);
  vrp_lsi.Solve();

  LocalSearchInterIntraSolution vrp_lsii(p);
  vrp_lsii.Solve();

  TabuSearchSolution vrp_ts(p);
  vrp_ts.Solve();

  // GASolution vrp_ga(p, 100, 5000);
  // vrp_ga.Solve();
}
