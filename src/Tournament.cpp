
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
      unsigned p1 = rand() % botBuilders.size();
      unsigned p2 = rand() % botBuilders.size();

      auto bot1 = botBuilders[p1]();
      auto bot2 = botBuilders[p2]();

      unsigned pr = playout(bot1.get(), bot2.get());
      unsigned wi = pr == 0 ? p1 : p2;

      wins[wi]++;
      played[p1]++;
      played[p2]++;
    }

    return vmap<unsigned, float>(wins, [&played](unsigned numWins, unsigned i) {
      return numWins / static_cast<float>(played[i]);
    });
  }

  unsigned playout(Bot* bot1, Bot* bot2) {
    return 0;
  }
};

Tournament::Tournament() : impl(new TournamentImpl()) {};
Tournament::~Tournament() = default;

vector<float> Tournament::RunTournament(const vector<function<uptr<naivebot::NaiveBot>()>> &botBuilders,
                                        unsigned rounds) {
  return impl->RunTournament(botBuilders, rounds);
}
