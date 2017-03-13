
#include "FastBot.hpp"
#include "mcts/MCTS.hpp"
#include <cassert>
#include <sstream>

using namespace fastbot;

static constexpr unsigned MIN_MS_PER_MOVE = 10;
static constexpr unsigned MAX_MS_PER_MOVE = 1000;
static constexpr unsigned REMAINING_MS_USE_RATIO = 20;

struct FastBot::FastBotImpl {
  unsigned char botId;
  unsigned millisecondsPerMove;
  unsigned millisecondsRemaining;
  Spec spec;

  FastBotImpl(unsigned millisecondsPerMove, const Spec &spec)
      : botId(1), millisecondsPerMove(millisecondsPerMove), millisecondsRemaining(0), spec(spec){};

  pair<int, int> ChooseAction(const string &field) {
    auto action = ChooseAction(parseState(field));
    return make_pair(action % 9, action / 9);
  }

  Action ChooseAction(const State &state) {
    unsigned msPerMove = millisecondsPerMove;
    // if (millisecondsPerMove < millisecondsRemaining) {
    //   msPerMove += (millisecondsRemaining - millisecondsPerMove) / REMAINING_MS_USE_RATIO;
    // }
    //
    // msPerMove = max(msPerMove, MIN_MS_PER_MOVE);
    // msPerMove = min(msPerMove, MAX_MS_PER_MOVE);

    mcts::MCTS mcts(msPerMove, spec);
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

    // TODO: fixme.
    return State(fieldCells, botId, -1);
  }

  std::vector<std::string> &split(const std::string &s, char delim,
                                  std::vector<std::string> &elems) {
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

FastBot::FastBot(unsigned microsecondsPerMove, const Spec &spec)
    : impl(new FastBotImpl(microsecondsPerMove, spec)) {}

FastBot::~FastBot() = default;

void FastBot::SetBotId(unsigned char botId) {
  assert(botId == 1 || botId == 2);
  impl->botId = botId;
}

unsigned char FastBot::GetBotId(void) const { return impl->botId; }

void FastBot::SetTimeRemaining(unsigned milliseconds) {
  impl->millisecondsRemaining = milliseconds;
}

pair<int, int> FastBot::ChooseAction(const string &field) { return impl->ChooseAction(field); }

Action FastBot::ChooseAction(const State &state) { return impl->ChooseAction(state); }
