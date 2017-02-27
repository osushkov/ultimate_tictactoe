
#include "Tournament.hpp"
#include "util/Common.hpp"

#include <mutex>
#include <thread>

static constexpr unsigned NUM_THREADS = 20;

static naivebot::State convert(const fastbot::State &otherState, unsigned char naivebotId) {
  array<naivebot::CellState, naivebot::NUM_CELLS> fieldCells;
  for (unsigned i = 0; i < naivebot::NUM_CELLS; i++) {
    auto ofs = otherState.GetCellState(i);

    switch (ofs) {
    case fastbot::CellState::EMPTY:
      fieldCells[i] = naivebot::CellState::EMPTY;
      break;
    case fastbot::CellState::NAUGHT:
    case fastbot::CellState::CROSS:
      fieldCells[i] = naivebotId == static_cast<unsigned char>(ofs)
                          ? naivebot::CellState::MY_TOKEN
                          : naivebot::CellState::OPPONENT_TOKEN;
      break;
    }
  }
  return naivebot::State(fieldCells);
}

static fastbot::State convert(const naivebot::State &otherState, unsigned char fastbotId) {
  array<fastbot::CellState, fastbot::NUM_CELLS> fieldCells;
  for (unsigned i = 0; i < fastbot::NUM_CELLS; i++) {
    auto ofs = otherState.Get(i);

    switch (ofs) {
    case naivebot::CellState::EMPTY:
      fieldCells[i] = fastbot::CellState::EMPTY;
      break;
    case naivebot::CellState::MY_TOKEN:
      fieldCells[i] = static_cast<fastbot::CellState>(fastbotId);
      break;
    case naivebot::CellState::OPPONENT_TOKEN:
      unsigned char otherbotId = fastbotId == 1 ? 2 : 1;
      fieldCells[i] = static_cast<fastbot::CellState>(otherbotId);
      break;
    }
  }
  return fastbot::State(fieldCells, fastbotId);
}

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
        thread t([this, &botBuilders, rounds, &wins, &played, &mtx]{
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

  vector<float> RunTournament(const function<uptr<naivebot::NaiveBot>()> &naivebotBuilder,
                              const function<uptr<fastbot::FastBot>()> &fastbotBuilder,
                              unsigned rounds) {

    vector<unsigned> wins(2, 0);
    vector<unsigned> played(2, 0);

    for (unsigned i = 0; i < rounds; i++) {
      cout << "round: " << i << endl;
      auto bot1 = naivebotBuilder();
      bot1->SetBotId(1);

      auto bot2 = fastbotBuilder();
      bot2->SetBotId(2);

      int pr = playoutNaiveFast(bot1.get(), bot2.get());
      played[0]++;
      played[1]++;

      if (pr != 0) {
        unsigned wi = pr == -1 ? 0 : 1;
        wins[wi]++;
      }
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

  int playoutNaiveFast(naivebot::NaiveBot *bot1, fastbot::FastBot *bot2) {
    naivebot::State curState;
    int curPlayer = rand() % 2;

    while (!curState.IsTerminal()) {
      if (curPlayer == 0) {
        auto action = bot1->ChooseAction(curState);
        curState = curState.SuccessorState(action);
      } else {
        fastbot::State fbCurState = convert(curState, bot2->GetBotId());
        auto action = bot2->ChooseAction(fbCurState);
        fbCurState = fbCurState.SuccessorState(action);
        curState = convert(fbCurState, bot1->GetBotId());
      }
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

vector<float> Tournament::RunTournament(const function<uptr<naivebot::NaiveBot>()> &naivebotBuilder,
                                        const function<uptr<fastbot::FastBot>()> &fastbotBuilder,
                                        unsigned rounds) {
  return impl->RunTournament(naivebotBuilder, fastbotBuilder, rounds);
}
