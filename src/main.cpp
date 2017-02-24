
#include <cstdlib>
#include <iostream>
#include "Tournament.hpp"
#include "BotIO.hpp"
#include "naivebot/NaiveBot.hpp"

using namespace std;

static void runNaiveTournament(void) {
  vector<function<uptr<naivebot::NaiveBot>()>> bots;
  bots.push_back([]() {
    return make_unique<naivebot::NaiveBot>(10000, false);
  });
  bots.push_back([]() {
    return make_unique<naivebot::NaiveBot>(10000, true);
  });

  Tournament tournament;
  vector<float> pWin = tournament.RunTournament(bots, 1);

  cout << "pwin:" << endl;
  for (unsigned i = 0; i < pWin.size(); i++) {
    cout << i << " : " << pWin[i] << endl;
  }
}

static void runNaiveVsFastTournament(void) {
  function<uptr<naivebot::NaiveBot>()> bot1 = []() {
    return make_unique<naivebot::NaiveBot>(50000, false);
  };
  function<uptr<fastbot::FastBot>()> bot2 = []() {
    return make_unique<fastbot::FastBot>(50000, false);
  };

  Tournament tournament;
  vector<float> pWin = tournament.RunTournament(bot1, bot2, 100);

  cout << "pwin:" << endl;
  for (unsigned i = 0; i < pWin.size(); i++) {
    cout << i << " : " << pWin[i] << endl;
  }
}

int main() {
  srand(1337);

  cout << "hello world" << endl;
  runNaiveVsFastTournament();

  // auto bot = make_unique<naivebot::NaiveBot>();
  //
  // BotIO botIO(std::move(bot));
  // botIO.Loop();

  return 0;
}
