
#pragma once

#include "../../util/Common.hpp"
#include "../Action.hpp"
#include "../State.hpp"
#include "../Rules.hpp"

// An action and the probability of winning by taking that action.
using ActionUtility = pair<Action *, double>;

namespace naivebot {
namespace mcts {

// Monte-Carlo Tree Search.
class MCTS {
public:
  MCTS();
  virtual ~MCTS();

  // Sorted list of action utilities.
  vector<ActionUtility> ComputeUtilities(State *curState);

private:
  struct MCTSImpl;
  uptr<MCTSImpl> impl;
};
}
}
