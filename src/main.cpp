#include "main.hpp"

int main(){
  Problem p(1000,4,25,125,1000);

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

  // LSSwapSolution vrp_lsss(p);
  // vrp_lsss.Solve();
  //
  // TSSolution vrp_ts(p);
  // vrp_ts.Solve();
}
