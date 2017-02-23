
#include "Tournament.hpp"
#include "util/Common.hpp"

struct Tournament::TournamentImpl {
  TournamentImpl() = default;
  ~TournamentImpl() = default;

  vector<float> RunTournament(const vector<function<uptr<naivebot::NaiveBot>()>> &botBuilders,
                              unsigned rounds) {

    vector<unsigned> wins(botBuilders.size(), 0);
    vector<unsigned> played(botBuilders.size(), 0);

    for (unsigned i = 0; i < rounds; i++) {
      cout << "round: " << i << endl;
      unsigned p1 = rand() % botBuilders.size();
      unsigned p2 = rand() % botBuilders.size();

      auto bot1 = botBuilders[p1]();
      auto bot2 = botBuilders[p2]();

      int pr = playout(bot1.get(), bot2.get());
      played[p1]++;
      played[p2]++;

      if (pr != 0) {
        unsigned wi = pr == -1 ? p1 : p2;
        wins[wi]++;
      }
    }

    return vmap<unsigned, float>(wins, [&played](unsigned numWins, unsigned i) {
      return played[i] == 0 ? 0.0 : (numWins / static_cast<float>(played[i]));
    });
  }

  int playout(naivebot::NaiveBot* bot1, naivebot::NaiveBot* bot2) {
    naivebot::State curState;
    int curPlayer = 0;

    while (!curState.IsTerminal()) {
    //   curState.Output(cout);
    //   getchar();
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

Tournament::Tournament() : impl(new TournamentImpl()) {};
Tournament::~Tournament() = default;

vector<float> Tournament::RunTournament(const vector<function<uptr<naivebot::NaiveBot>()>> &botBuilders,
                                        unsigned rounds) {
  return impl->RunTournament(botBuilders, rounds);
}
