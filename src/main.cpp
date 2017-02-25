
#include "BotIO.hpp"
#include "Tournament.hpp"
#include "naivebot/NaiveBot.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

static void runFastTournament(void) {
  vector<fastbot::Spec> botSpecs;
  botSpecs.push_back(fastbot::Spec(0.1f, 0.4f, 0.3f, 0.2f));
  botSpecs.push_back(fastbot::Spec(0.1f, 0.1f, 0.3f, 0.2f));
  botSpecs.push_back(fastbot::Spec(0.1f, 0.2f, 0.3f, 0.2f));

  vector<function<uptr<fastbot::FastBot>()>> bots;
  for (const auto &spec : botSpecs) {
    bots.push_back([&spec]() { return make_unique<fastbot::FastBot>(50000, spec); });
  }

  Tournament tournament;
  vector<float> pWin = tournament.RunTournament(bots, 50);

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
    return make_unique<fastbot::FastBot>(50000, fastbot::Spec(0.1f, 0.1f, 0.3f, 0.2f));
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
  // runNaiveVsFastTournament();
  runFastTournament();
  // auto bot = make_unique<naivebot::NaiveBot>();
  //
  // BotIO botIO(std::move(bot));
  // botIO.Loop();

  return 0;
}
