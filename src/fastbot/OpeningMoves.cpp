
#include "OpeningMoves.hpp"

using namespace fastbot;
using namespace std;

static const vector<pair<unsigned char, unsigned char>> moves2 = {};
static const vector<pair<pair<unsigned char, unsigned char>, unsigned char>> moves3 = {};

static unsigned countNumMovesMade(const State &state) {
  unsigned result = 0;
  for (unsigned i = 0; i < NUM_CELLS; i++) {
    if (state.GetCellState(i) != CellState::EMPTY) {
      result++;
    }
  }
  return result;
}

bool openingmoves::HaveActionFor(const State &state) { return countNumMovesMade(state) <= 3; }

Action openingmoves::GetActionFor(const State &state) {
  unsigned numMovesMade = countNumMovesMade(state);

  switch (numMovesMade) {
  case 0:
    return Action(40);
  case 1:
    return state.GetCellState(40) == CellState::EMPTY ? Action(40) : Action(10);
  case 2:
    assert(static_cast<unsigned char>(state.GetCellState(40)) == state.GetMySymbol());
    for (const auto &m : moves2) {
      if (state.GetCellState(m.first) != CellState::EMPTY) {
        return Action(m.second);
      }
    }
    break;
  case 3:
    assert(static_cast<unsigned char>(state.GetCellState(40)) == state.GetMySymbol() ||
           static_cast<unsigned char>(state.GetCellState(10)) == state.GetMySymbol());
    CellState opponentSymbol =
        (state.GetMySymbol() == static_cast<unsigned char>(CellState::NAUGHT)) ? CellState::CROSS
                                                                               : CellState::NAUGHT;
    for (const auto &m : moves3) {
      if (state.GetCellState(m.first.first) == opponentSymbol &&
          state.GetCellState(m.first.second) == opponentSymbol &&
          state.GetCellState(m.second) == CellState::EMPTY) {
        return Action(m.second);
      }
    }
    break;
  }
  return Action(100);
}
