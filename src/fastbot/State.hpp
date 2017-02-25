
#pragma once

#include "../util/Common.hpp"
#include "Action.hpp"
#include <array>
#include <iostream>
#include <ostream>
#include <vector>

namespace fastbot {

constexpr unsigned NUM_CELLS = 81;
constexpr unsigned NUM_TOP_CELLS = 9;

enum class CellState : unsigned char { EMPTY = 0, NAUGHT = 1, CROSS = 2 };
std::ostream &operator<<(std::ostream &stream, const CellState &cs);

enum class TopCellState : unsigned char { UNDECIDED = 0, NAUGHT = 1, CROSS = 2, DRAW = 3 };
std::ostream &operator<<(std::ostream &stream, const TopCellState &tcs);

class State {
  array<CellState, NUM_CELLS> cells;
  array<TopCellState, NUM_TOP_CELLS> topCells;

  unsigned char mySymbol; // 1 = NAUGHT, 2 = CROSS

  bool isTerminal;
  bool isWin;
  bool isLoss;

public:
  State(unsigned char mySymbol);
  State(const array<CellState, NUM_CELLS> &fieldCells, unsigned char mySymbol);
  State(const State &other);

  ~State() = default;

  State &operator=(const State &other);
  bool operator==(const State &other) const;

  // Print this state to the output stream.
  void Output(std::ostream &out) const;

  // Returns a list of all possible actions in this state.
  vector<Action> AvailableActions(void) const;
  Action ChooseRandomAction(void) const;

  // Returns a successor state when the given action is applied in this state.
  // This function can be stochastic and return a different State every time it is called.
  State SuccessorState(Action action) const;

  CellState GetCellState(unsigned cellIndex) const;
  unsigned char GetMySymbol(void) const;

  bool IsTerminal(void) const;
  bool IsWin(void) const;
  bool IsLoss(void) const;

private:
  // Whenever we make a move and want another agent to make a move, then we should "flip" the
  // board such that what are currently "our" tokens become "oppponent" tokens, and vice versa.
  void flipState(void);

  void updateFlags(void);
};
}
