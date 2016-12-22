
#pragma once

#include "Action.hpp"
#include "../util/Common.hpp"
#include <iostream>
#include <ostream>
#include <vector>

namespace naivebot {

class State {
public:
  State();
  ~State();

  uptr<State> Clone(void) const = 0;

  bool operator==(const State &obj) const;

  // Returns a hash of this state.
  size_t HashCode() const;

  // Print this state to the output stream.
  void Output(std::ostream &out) const;

  // Returns a list of all possible actions in this state.
  vector<uptr<Action>> AvailableActions(void) const;

  // Returns a successor state when the given action is applied in this state.
  // This function can be stochastic and return a different State every time it is called.
  uptr<State> SuccessorState(const Action &action) const;

  // TODO: do i need this?
  struct StatePEquals {
    bool operator()(State *s1, State *s2) const { return *s1 == *s2; }
  };
};
}

namespace std {
template <> struct hash<State *> {
  inline size_t operator()(naivebot::State const *state) const { return state->HashCode(); }
};
}
