
#include <cstdlib>
#include <iostream>
#include "Tournament.hpp"
#include "BotIO.hpp"
#include "naivebot/NaiveBot.hpp"

using namespace std;

static void runTournament(void) {
  vector<function<uptr<naivebot::NaiveBot>()>> bots;
  bots.push_back([]() {
    return make_unique<naivebot::NaiveBot>();
  });

  Tournament tournament;
  vector<float> pWin = tournament.RunTournament(bots, 20);

  cout << "pwin:" << endl;
  for (unsigned i = 0; i < pWin.size(); i++) {
    cout << i << " : " << pWin[i] << endl;
  }
}

int main() {
  srand(1337);

  cout << "hello world" << endl;
  runTournament();

  // auto bot = make_unique<naivebot::NaiveBot>();
  //
  // BotIO botIO(std::move(bot));
  // botIO.Loop();

  return 0;
}
