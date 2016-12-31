
#pragma once

#include "../util/Common.hpp"
#include "Action.hpp"
#include <array>
#include <iostream>
#include <ostream>
#include <vector>

namespace naivebot {

constexpr unsigned NUM_CELLS = 81;
constexpr unsigned NUM_TOP_CELLS = 9;

enum class CellState { MY_TOKEN = 0, OPPONENT_TOKEN = 1, EMPTY = 2 };
std::ostream &operator<<(std::ostream &stream, const CellState &cs);

class State {
  array<CellState, NUM_CELLS> cells;
  array<CellState, NUM_TOP_CELLS> topCells;

public:
  State();
  State(const State &other);

  ~State() = default;

  bool operator==(const State &other) const;

  // Returns a hash of this state.
  size_t HashCode() const;

  // Print this state to the output stream.
  void Output(std::ostream &out) const;

  // Returns a list of all possible actions in this state.
  vector<Action> AvailableActions(void) const;

  // Returns a successor state when the given action is applied in this state.
  // This function can be stochastic and return a different State every time it is called.
  State SuccessorState(const Action &action) const;

  // Whenever we make a move and want another agent to make a move, then we should "flip" the
  // board such that what are currently "our" tokens become "oppponent" tokens, and vice versa.
  void FlipState(void);

  // TODO: do i need this?
  struct StatePEquals {
    bool operator()(State *s1, State *s2) const { return *s1 == *s2; }
  };
};
}

namespace std {
template <> struct hash<naivebot::State *> {
  inline size_t operator()(naivebot::State const *state) const { return state->HashCode(); }
};
}
