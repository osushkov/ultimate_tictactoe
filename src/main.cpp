
#include "BotIO.hpp"
#include "Tournament.hpp"
#include "fastbot/FastBot.hpp"
#include "fastbot/Spec.hpp"
#include "util/Util.hpp"
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

static constexpr unsigned MS_PER_MOVE = 150;

static fastbot::Spec randomSpec(void) {
  float minExplorationWeightA = 0.0f, maxExplorationWeightA = 2.0f;
  float minExplorationWeightC = 0.0f, maxExplorationWeightC = 2.0f;
  float minRandomWeight = 0.0f, maxRandomWeight = 1.0f;
  float minPRandomSelect = 0.0f, maxPRandomSelect = 1.0f;

  return fastbot::Spec(Util::RandInterval(minExplorationWeightA, maxExplorationWeightA),
                       Util::RandInterval(minExplorationWeightC, maxExplorationWeightC),
                       Util::RandInterval(minRandomWeight, maxRandomWeight),
                       Util::RandInterval(minPRandomSelect, maxPRandomSelect));
}

static fastbot::Spec initialSpec(void) {
  return fastbot::Spec(0.546502f, 0.377958f, 0.14554f, 0.178079f);
}

static pair<fastbot::Spec, float> runFastTournament(const fastbot::Spec &seedSpec,
                                                    unsigned opponents, unsigned rounds) {
  vector<fastbot::Spec> botSpecs;
  botSpecs.push_back(seedSpec);
  for (unsigned i = 0; i < opponents; i++) {
    botSpecs.push_back(randomSpec());
  }

  vector<function<uptr<fastbot::FastBot>()>> bots;
  for (const auto &spec : botSpecs) {
    bots.push_back([&spec]() { return make_unique<fastbot::FastBot>(MS_PER_MOVE, spec); });
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

static fastbot::Spec betterSpec(const fastbot::Spec &specA, const fastbot::Spec &specB,
                                unsigned rounds) {
  vector<fastbot::Spec> botSpecs;
  botSpecs.push_back(specA);
  botSpecs.push_back(specB);

  vector<function<uptr<fastbot::FastBot>()>> bots;
  for (const auto &spec : botSpecs) {
    bots.push_back([&spec]() { return make_unique<fastbot::FastBot>(MS_PER_MOVE, spec); });
  }

  Tournament tournament;
  vector<float> pWin = tournament.RunTournament(bots, rounds);
  cout << pWin[0] << " : " << pWin[1] << endl;

  return pWin[0] > pWin[1] ? specA : specB;
}

static void calculateOpeningMoves(void) {
  vector<unsigned char> madeMoves;
  for (unsigned char i = 0; i < 81; i++) {
    if (i == 40) {
      continue;
    }

    if (((i / 9) / 3) == 1 && ((i % 9) / 3) == 1) {
      madeMoves.emplace_back(i);
    }
  }

  vector<fastbot::Action> bestMove(madeMoves.size(), 99);
  vector<thread> threads;

  const unsigned NUM_THREADS = 20;
  for (unsigned ti = 0; ti < NUM_THREADS; ti++) {
    thread t([ti, &madeMoves, &bestMove] {
      const unsigned msPerMove = 1000 * 30;
      uptr<fastbot::FastBot> bot = make_unique<fastbot::FastBot>(
          msPerMove, fastbot::Spec(0.546502f, 0.377958f, 0.14554f, 0.178079f));
      bot->SetBotId(1);

      unsigned si = (ti * madeMoves.size()) / NUM_THREADS;
      unsigned ei = ((ti + 1) * madeMoves.size()) / NUM_THREADS;
      for (unsigned i = si; i < ei; i++) {
        fastbot::State state(1);
        state = state.SuccessorState(fastbot::Action(40));
        state = state.SuccessorState(fastbot::Action(madeMoves[i]));
        bestMove[i] = bot->ChooseAction(state);
      }
    });

    threads.push_back(move(t));
  }

  for (auto &t : threads) {
    t.join();
  }

  for (unsigned i = 0; i < madeMoves.size(); i++) {
    cout << "{" << static_cast<int>(madeMoves[i]) << "," << static_cast<int>(bestMove[i]) << "},";
  }
  cout << endl;

  /*
  vector<pair<unsigned char, unsigned char>> madeMoves;
  for (unsigned char i = 0; i < 81; i++) {
    for (unsigned char j = 0; j < 81; j++) {
      madeMoves.emplace_back(i, j);
    }
  }

  vector<fastbot::Action> bestMove(madeMoves.size(), 99);
  vector<thread> threads;

  const unsigned NUM_THREADS = 20;
  for (unsigned ti = 0; ti < NUM_THREADS; ti++) {
    thread t([ti, &madeMoves, &bestMove] {
      const unsigned msPerMove = 1000 * 30;
      uptr<fastbot::FastBot> bot = make_unique<fastbot::FastBot>(
          msPerMove, fastbot::Spec(0.546502f, 0.377958f, 0.14554f, 0.178079f));
      bot->SetBotId(2);

      unsigned si = (ti * madeMoves.size()) / NUM_THREADS;
      unsigned ei = ((ti + 1) * madeMoves.size()) / NUM_THREADS;
      for (unsigned i = si; i < ei; i++) {
        if (madeMoves[i].first == madeMoves[i].second) {
          continue;
        }

        fastbot::State state(1);
        state = state.SuccessorState(fastbot::Action(madeMoves[i].first));

        fastbot::Action myAction(madeMoves[i].first == 40 ? 10 : 40);
        if (myAction == madeMoves[i].second) {
          continue;
        }

        state = state.SuccessorState(myAction);
        state = state.SuccessorState(fastbot::Action(madeMoves[i].second));

        bestMove[i] = bot->ChooseAction(state);
      }
    });

    threads.push_back(move(t));
  }

  for (auto &t : threads) {
    t.join();
  }

  for (unsigned i = 0; i < madeMoves.size(); i++) {
    cout << "{{" << static_cast<int>(madeMoves[i].first) << ","
         << static_cast<int>(madeMoves[i].second) << "}," << static_cast<int>(bestMove[i]) << "}"
         << endl;
  }
  */
}

int main() {
  srand(time(NULL));
  // srand(1337);

  calculateOpeningMoves();

  // fastbot::Spec bestSpec = initialSpec();
  //
  // for (unsigned i = 0; i < 100; i++) {
  //   auto r = runFastTournament(bestSpec, 4, 500);
  //   cout << "i: " << i << " " << r.second << endl;
  //
  //   if (r.first != bestSpec) {
  //     bestSpec = betterSpec(bestSpec, r.first, 200);
  //
  //     cout << "best: " << r.second << endl;
  //     cout << bestSpec.explorationWeightA << endl;
  //     cout << bestSpec.explorationWeightC << endl;
  //     cout << bestSpec.randomWeight << endl;
  //     cout << bestSpec.pRandomSelect << endl << endl;
  //   }
  // }

  //
  // BotIO botIO(std::move(bot));
  // botIO.Loop();

  return 0;
}
