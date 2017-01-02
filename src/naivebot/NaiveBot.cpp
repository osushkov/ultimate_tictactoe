
#include "NaiveBot.hpp"
#include "mcts/MCTS.hpp"

using namespace naivebot;

struct NaiveBot::NaiveBotImpl {
  NaiveBotImpl() = default;

  pair<int, int> ChooseAction(const string &field) {
    array<CellState, NUM_CELLS> fieldCells;

    State state(fieldCells);
    mcts::MCTS mcts;

    vector<mcts::ActionUtility> actions = mcts.ComputeUtilities(state);
    return make_pair(actions.front().first.x, actions.front().first.y);
  }
};

NaiveBot::NaiveBot() : impl(new NaiveBotImpl()) {}
NaiveBot::~NaiveBot() = default;

pair<int, int> NaiveBot::ChooseAction(const string &field) { return impl->ChooseAction(field); }
