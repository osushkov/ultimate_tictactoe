
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
    bots.push_back([&spec]() {
      auto result = make_unique<fastbot::FastBot>(spec);
      result->SetTimePerMove(MS_PER_MOVE);
      return result;
    });
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
    bots.push_back([&spec]() {
      auto result = make_unique<fastbot::FastBot>(spec);
      result->SetTimePerMove(MS_PER_MOVE);
      return result;
    });
  }

  Tournament tournament;
  vector<float> pWin = tournament.RunTournament(bots, rounds);
  cout << pWin[0] << " : " << pWin[1] << endl;

  return pWin[0] > pWin[1] ? specA : specB;
}

static const vector<pair<unsigned char, unsigned char>> moves1 = {
    {0, 20},  {1, 22},  {2, 8},   {3, 0},   {4, 3},   {5, 24},  {6, 18},  {7, 21},  {8, 24},
    {9, 47},  {10, 40}, {11, 44}, {12, 38}, {13, 40}, {14, 34}, {15, 28}, {16, 40}, {17, 44},
    {18, 55}, {19, 77}, {20, 70}, {21, 72}, {22, 59}, {23, 80}, {24, 72}, {25, 75}, {26, 80},
    {27, 2},  {28, 12}, {29, 24}, {30, 10}, {31, 23}, {32, 24}, {33, 0},  {34, 12}, {35, 8},
    {36, 27}, {37, 40}, {38, 44}, {39, 46}, {40, 30}, {41, 44}, {42, 36}, {43, 48}, {44, 34},
    {45, 56}, {46, 77}, {47, 78}, {48, 56}, {49, 68}, {50, 78}, {51, 56}, {52, 68}, {53, 60},
    {54, 1},  {55, 14}, {56, 25}, {57, 9},  {58, 14}, {59, 24}, {60, 10}, {61, 22}, {62, 8},
    {63, 46}, {64, 40}, {65, 44}, {66, 28}, {67, 32}, {68, 51}, {69, 36}, {70, 40}, {71, 33},
    {72, 64}, {73, 59}, {74, 61}, {75, 74}, {76, 58}, {77, 60}, {78, 63}, {79, 68}, {80, 60}};

static void calculateOpeningMoves(void) {
  vector<unsigned char> firstMove;
  for (unsigned char i = 0; i < 81; i++) {
    firstMove.emplace_back(i);
  }

  vector<pair<pair<unsigned char, unsigned char>, unsigned char>> bestMove;
  vector<thread> threads;
  mutex mtx;

  const unsigned NUM_THREADS = 4;
  for (unsigned ti = 0; ti < NUM_THREADS; ti++) {
    thread t([ti, &firstMove, &bestMove, &mtx] {
      const unsigned msPerMove = 1000 * 30;
      uptr<fastbot::FastBot> bot =
          make_unique<fastbot::FastBot>(fastbot::Spec(0.546502f, 0.377958f, 0.14554f, 0.178079f));
      bot->SetTimePerMove(msPerMove);
      bot->SetBotId(2);

      unsigned si = (ti * firstMove.size()) / NUM_THREADS;
      unsigned ei = ((ti + 1) * firstMove.size()) / NUM_THREADS;
      for (unsigned i = si; i < ei; i++) {
        fastbot::State state(1);
        state = state.SuccessorState(fastbot::Action(firstMove[i]));

        for (const auto &rm : moves1) {
          if (rm.first == firstMove[i]) {
            state = state.SuccessorState(fastbot::Action(rm.second));
            break;
          }
        }

        auto availableActions = state.AvailableActions();
        for (auto action : availableActions) {
          fastbot::State sampleState = state.SuccessorState(action);
          auto myAction = bot->ChooseAction(sampleState);

          std::unique_lock<std::mutex> lock(mtx);
          bestMove.emplace_back(make_pair(firstMove[i], action), myAction);
        }
      }
    });

    threads.push_back(move(t));
  }

  for (auto &t : threads) {
    t.join();
  }

  for (const auto &entry : bestMove) {
    cout << "{{" << static_cast<int>(entry.first.first) << ","
         << static_cast<int>(entry.first.second) << "}," << static_cast<int>(entry.second) << "}"
         << endl;
  }
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
