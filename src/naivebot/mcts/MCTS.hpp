
#pragma once

#include "../../util/Common.hpp"
#include "../Action.hpp"
#include "../Rules.hpp"
#include "../State.hpp"

namespace naivebot {
namespace mcts {

// An action and the probability of winning by taking that action.
using ActionUtility = pair<Action, double>;

// Monte-Carlo Tree Search.
class MCTS {
public:
  MCTS();
  virtual ~MCTS();

  // Sorted list of action utilities.
  vector<ActionUtility> ComputeUtilities(const State &curState);

private:
  struct MCTSImpl;
  uptr<MCTSImpl> impl;
};
}
}
