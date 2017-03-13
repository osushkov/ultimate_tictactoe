
#include "Tournament.hpp"
#include "util/Common.hpp"

#include <mutex>
#include <thread>

static constexpr unsigned NUM_THREADS = 20;

struct Tournament::TournamentImpl {
  TournamentImpl() = default;
  ~TournamentImpl() = default;

  vector<float> RunTournament(const vector<function<uptr<fastbot::FastBot>()>> &botBuilders,
                              unsigned rounds) {

    vector<unsigned> wins(botBuilders.size(), 0);
    vector<unsigned> played(botBuilders.size(), 0);
    mutex mtx;

    vector<thread> threads;
    for (unsigned i = 0; i < NUM_THREADS; i++) {
      thread t([this, &botBuilders, rounds, &wins, &played, &mtx] {
        for (unsigned i = 0; i < rounds / NUM_THREADS; i++) {
          unsigned p1 = rand() % botBuilders.size();
          unsigned p2 = rand() % botBuilders.size();

          auto bot1 = botBuilders[p1]();
          bot1->SetBotId(1);

          auto bot2 = botBuilders[p2]();
          bot2->SetBotId(2);

          int pr = playoutFast(bot1.get(), bot2.get());

          std::unique_lock<std::mutex> lock(mtx);
          played[p1]++;
          played[p2]++;

          if (pr != 0) {
            unsigned wi = pr == -1 ? p1 : p2;
            wins[wi]++;
          }
        }
      });
      threads.push_back(move(t));
    }

    for (auto &t : threads) {
      t.join();
    }

    return vmap<unsigned, float>(wins, [&played](unsigned numWins, unsigned i) {
      return played[i] == 0 ? 0.0 : (numWins / static_cast<float>(played[i]));
    });
  }

  int playoutFast(fastbot::FastBot *bot1, fastbot::FastBot *bot2) {
    fastbot::State curState(bot1->GetBotId());
    int curPlayer = 0;

    while (!curState.IsTerminal()) {
      auto action = (curPlayer == 0 ? bot1 : bot2)->ChooseAction(curState);
      curState = curState.SuccessorState(action);
      curPlayer = 1 - curPlayer;
    }

    if (curState.IsWin()) {
      return curPlayer == 0 ? -1 : 1;
    } else if (curState.IsLoss()) {
      return curPlayer == 0 ? 1 : -1;
    } else {
      return 0;
    }
  }
};

Tournament::Tournament() : impl(new TournamentImpl()){};
Tournament::~Tournament() = default;

vector<float>
Tournament::RunTournament(const vector<function<uptr<fastbot::FastBot>()>> &botBuilders,
                          unsigned rounds) {
  return impl->RunTournament(botBuilders, rounds);
}
