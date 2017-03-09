
#include "BotIO.hpp"
#include "Tournament.hpp"
#include "util/Util.hpp"
#include "fastbot/FastBot.hpp"
#include "naivebot/NaiveBot.hpp"
#include "fastbot/Spec.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

static constexpr unsigned MICROSECONDS_PER_MOVE = 300000;

static fastbot::Spec randomSpec(void) {
    float minExplorationWeightA = 0.0f, maxExplorationWeightA = 3.0f;
    float minExplorationWeightC = 0.0f, maxExplorationWeightC = 3.0f;
    float minRandomWeight = 0.0f, maxRandomWeight = 2.0f;
    float minPRandomSelect = 0.0f, maxPRandomSelect = 1.0f;

    return fastbot::Spec(
        Util::RandInterval(minExplorationWeightA, maxExplorationWeightA),
        Util::RandInterval(minExplorationWeightC, maxExplorationWeightC),
        Util::RandInterval(minRandomWeight, maxRandomWeight),
        Util::RandInterval(minPRandomSelect, maxPRandomSelect)
    );
}

static fastbot::Spec initialSpec(void) {
  return fastbot::Spec(0.546502f, 0.377958f, 0.14554f, 0.178079f);
}

static pair<fastbot::Spec, float> runFastTournament(const fastbot::Spec &seedSpec, unsigned opponents, unsigned rounds) {
  vector<fastbot::Spec> botSpecs;
  botSpecs.push_back(seedSpec);
  for (unsigned i = 0; i < opponents; i++) {
      botSpecs.push_back(randomSpec());
  }

  vector<function<uptr<fastbot::FastBot>()>> bots;
  for (const auto &spec : botSpecs) {
    bots.push_back([&spec]() { return make_unique<fastbot::FastBot>(MICROSECONDS_PER_MOVE, spec); });
  }

  Tournament tournament;
  vector<float> pWin = tournament.RunTournament(bots, rounds);

  fastbot::Spec bestSpec;
  float bestPWin = 0.0f;

  for (unsigned i = 0; i < pWin.size(); i++) {
    if (pWin[i] >= bestPWin) {
        bestPWin = pWin[i];
        bestSpec = botSpecs[i];
    }
  }

  return make_pair(bestSpec, bestPWin);
}

static fastbot::Spec betterSpec(const fastbot::Spec &specA, const fastbot::Spec &specB, unsigned rounds) {
  vector<fastbot::Spec> botSpecs;
  botSpecs.push_back(specA);
  botSpecs.push_back(specB);

  vector<function<uptr<fastbot::FastBot>()>> bots;
  for (const auto &spec : botSpecs) {
    bots.push_back([&spec]() { return make_unique<fastbot::FastBot>(MICROSECONDS_PER_MOVE, spec); });
  }

  Tournament tournament;
  vector<float> pWin = tournament.RunTournament(bots, rounds);

  return pWin[0] > pWin[1] ? specA : specB;
}

static void runNaiveVsFastTournament(void) {
  function<uptr<naivebot::NaiveBot>()> bot1 = []() {
    return make_unique<naivebot::NaiveBot>(300000, false);
  };
  function<uptr<fastbot::FastBot>()> bot2 = []() {
    return make_unique<fastbot::FastBot>(300000, fastbot::Spec(0.546502f, 0.377958f, 0.14554f, 0.178079f));
  };

  Tournament tournament;
  vector<float> pWin = tournament.RunTournament(bot1, bot2, 100);

  cout << "pwin:" << endl;
  for (unsigned i = 0; i < pWin.size(); i++) {
    cout << i << " : " << pWin[i] << endl;
  }
}

static void calculateOpeningMoves(void) {
    unsigned msPerMove = 1000 * 1000 * 60 * 5;
    uptr<fastbot::FastBot> bot =
        make_unique<fastbot::FastBot>(msPerMove, fastbot::Spec(0.546502f, 0.377958f, 0.14554f, 0.178079f));

    fastbot::Action a = bot->ChooseAction(fastbot::State(0));
    cout << "best opening action: " << static_cast<int>(a) << endl;
}

int main() {
  srand(1337);

  calculateOpeningMoves();
  // runNaiveVsFastTournament();

  // fastbot::Spec bestSpec = initialSpec();
  //
  // for (unsigned i = 0; i < 100; i++) {
  //     auto r = runFastTournament(bestSpec, 4, 500);
  //
  //     if (r.first != bestSpec) {
  //       bestSpec = betterSpec(bestSpec, r.first, 200);
  //
  //       cout << "best: " << r.second << endl;
  //       cout << bestSpec.explorationWeightA << endl;
  //       cout << bestSpec.explorationWeightC << endl;
  //       cout << bestSpec.randomWeight << endl;
  //       cout << bestSpec.pRandomSelect << endl << endl;
  //   }
  // }

  //
  // BotIO botIO(std::move(bot));
  // botIO.Loop();

  return 0;
}
