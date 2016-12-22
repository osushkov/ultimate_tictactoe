
#pragma once

#include "../util/Common.hpp"
#include "State.hpp"

namespace naivebot {

class Rules {
public:
  Rules();
  ~Rules() = default;

  uptr<State> InitialState(void) const;

  // Returns whether it is possible to continue playing.
  bool IsTerminalState(const State &state) const;

  // Returns whether the current actor has won the game.
  bool IsWin(const State &state) const;

  // Returns whether the current actor has lost the game.
  bool IsLoss(const State &state) const;
};
}
