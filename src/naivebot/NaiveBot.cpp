
#include "NaiveBot.hpp"

using namespace naivebot;

struct NaiveBot::NaiveBotImpl {
  NaiveBotImpl() = default;

  pair<int, int> ChooseAction(const string &field) {
    //   MCTS mcts;
      //
    //   vector<ActionUtility> actions = mcts.ComputeUtilities(state);
    //   return actions.front().first->Clone();
      return make_pair(0, 0);
  }
};

NaiveBot::NaiveBot() : impl(new NaiveBotImpl()) {}
NaiveBot::~NaiveBot() = default;

pair<int, int> NaiveBot::ChooseAction(const string &field) {
  return impl->ChooseAction(field);
}
