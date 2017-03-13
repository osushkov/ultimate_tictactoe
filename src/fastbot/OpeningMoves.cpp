
#include "OpeningMoves.hpp"

using namespace fastbot;
using namespace std;

static const vector<pair<unsigned char, unsigned char>> moves1 = {
    {0, 20},  {1, 22},  {2, 8},   {3, 0},   {4, 3},   {5, 24},  {6, 18},  {7, 21},  {8, 24},
    {9, 47},  {10, 40}, {11, 44}, {12, 38}, {13, 40}, {14, 34}, {15, 28}, {16, 40}, {17, 44},
    {18, 55}, {19, 77}, {20, 70}, {21, 72}, {22, 59}, {23, 80}, {24, 72}, {25, 75}, {26, 80},
    {27, 2},  {28, 12}, {29, 24}, {30, 10}, {31, 23}, {32, 24}, {33, 0},  {34, 12}, {35, 8},
    {36, 27}, {37, 40}, {38, 44}, {39, 46}, {40, 30}, {41, 44}, {42, 36}, {43, 48}, {44, 34},
    {45, 56}, {46, 77}, {47, 78}, {48, 56}, {49, 68}, {50, 78}, {51, 56}, {52, 68}, {53, 60},
    {54, 1},  {55, 14}, {56, 25}, {57, 9},  {58, 14}, {59, 24}, {60, 10}, {61, 22}, {62, 8},
    {63, 46}, {64, 40}, {65, 44}, {66, 28}, {67, 32}, {68, 51}, {69, 36}, {70, 40}, {71, 33},
    {72, 64}, {73, 59}, {74, 61}, {75, 74}, {76, 58}, {77, 60}, {78, 63}, {79, 68}, {80, 60}};

static const vector<pair<unsigned char, unsigned char>> moves2 = {
    {30, 18}, {31, 12}, {32, 25}, {39, 27}, {41, 34}, {48, 74}, {49, 66}, {50, 78}};

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
