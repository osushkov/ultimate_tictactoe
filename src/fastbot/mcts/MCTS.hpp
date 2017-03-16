
#pragma once

#include "../../util/Common.hpp"
#include "../Action.hpp"
#include "../Spec.hpp"
#include "../State.hpp"

namespace fastbot {
namespace mcts {

// An action and the probability of winning by taking that action.
using ActionUtility = pair<Action, float>;

// Monte-Carlo Tree Search.
class MCTS {
public:
  MCTS(const Spec &spec);
  virtual ~MCTS();

  // Sorted list of action utilities.
  vector<ActionUtility> ComputeUtilities(const State &curState, unsigned timeoutMilliseconds);

private:
  struct MCTSImpl;
  uptr<MCTSImpl> impl;
};
}
}
