
#include "FastBot.hpp"
#include "OpeningMoves.hpp"
#include "mcts/MCTS.hpp"
#include <cassert>
#include <sstream>

using namespace fastbot;

static constexpr unsigned DEFAULT_MS_PER_MOVE = 500;
static constexpr unsigned MIN_MS_PER_MOVE = 10;
static constexpr unsigned MAX_MS_PER_MOVE = 1000;
static constexpr unsigned REMAINING_MS_USE_RATIO = 20;

struct FastBot::FastBotImpl {
  unsigned char botId;
  unsigned millisecondsPerMove;
  unsigned millisecondsBank;
  Spec spec;

  FastBotImpl(const Spec &spec)
      : botId(1), millisecondsPerMove(DEFAULT_MS_PER_MOVE), millisecondsBank(0), spec(spec){};

  pair<int, int> ChooseAction(const string &field, signed char topCellRestriction) {
    auto action = ChooseAction(parseState(field, topCellRestriction));
    return make_pair(action % 9, action / 9);
  }

  Action ChooseAction(const State &state) {
    if (openingmoves::HaveActionFor(state)) {
      Action lookupAction = openingmoves::GetActionFor(state);
      if (lookupAction < 81) {
        return lookupAction;
      }
    }

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

  State parseState(const string &field, signed char topCellRestriction) {
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

FastBot::FastBot(const Spec &spec) : impl(new FastBotImpl(spec)) {}

FastBot::~FastBot() = default;

void FastBot::SetBotId(unsigned char botId) {
  assert(botId == 1 || botId == 2);
  impl->botId = botId;
}

unsigned char FastBot::GetBotId(void) const { return impl->botId; }

void FastBot::SetTimePerMove(unsigned milliseconds) { impl->millisecondsPerMove = milliseconds; }
void FastBot::SetTimeBank(unsigned milliseconds) { impl->millisecondsBank = milliseconds; }

pair<int, int> FastBot::ChooseAction(const string &field, signed char topCellRestriction) {
  return impl->ChooseAction(field, topCellRestriction);
}

Action FastBot::ChooseAction(const State &state) { return impl->ChooseAction(state); }
