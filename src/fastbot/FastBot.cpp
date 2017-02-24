
#include "FastBot.hpp"
#include "mcts/MCTS.hpp"
#include <sstream>
#include <cassert>

using namespace fastbot;

struct FastBot::FastBotImpl {
  unsigned char botId;
  unsigned microsecondsPerMove;
  bool useEGreedy;

  FastBotImpl(unsigned microsecondsPerMove, bool useEGreedy) :
        botId(1), microsecondsPerMove(microsecondsPerMove), useEGreedy(useEGreedy) {};

  pair<int, int> ChooseAction(const string &field) {
    auto action = ChooseAction(parseState(field));
    return make_pair(action % 9, action / 9);
  }

  Action ChooseAction(const State &state) {
    mcts::MCTS mcts(microsecondsPerMove, useEGreedy);
    vector<mcts::ActionUtility> actions = mcts.ComputeUtilities(state);
    return actions.front().first;
  }

  State parseState(const string &field) {
    std::vector<std::string> rawValues;
    split(field, ',', rawValues);
    assert(rawValues.size() == NUM_CELLS);

    array<CellState, NUM_CELLS> fieldCells;
    for (unsigned i = 0; i < rawValues.size(); i++) {
      int v = stringToInt(rawValues[i]);
      assert(v >= 0 && v <= 2);

      fieldCells[i] = v == 0 ? CellState::EMPTY : static_cast<CellState>(v);
    }

    return State(fieldCells, botId);
  }

  std::vector<std::string> &split(
          const std::string &s, char delim, std::vector<std::string> &elems) {
      std::stringstream ss(s);
      std::string item;
      elems.clear();
      while (std::getline(ss, item, delim)) {
          elems.push_back(item);
      }
      return elems;
  }

  int stringToInt(const std::string &s) {
      std::istringstream ss(s);
      int result;
      ss >> result;
      return result;
  }
};

FastBot::FastBot(unsigned microsecondsPerMove, bool useEGreedy) :
        impl(new FastBotImpl(microsecondsPerMove, useEGreedy)) {}
FastBot::~FastBot() = default;

void FastBot::SetBotId(unsigned char botId) {
  assert(botId == 1 || botId == 2);
  impl->botId = botId;
}

unsigned char FastBot::GetBotId(void) const { return impl->botId; }

pair<int, int> FastBot::ChooseAction(const string &field) { return impl->ChooseAction(field); }

Action FastBot::ChooseAction(const State &state) {
  return impl->ChooseAction(state);
}
