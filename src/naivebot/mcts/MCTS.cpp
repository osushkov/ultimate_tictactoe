
#include "MCTS.hpp"
#include "../../util/Timer.hpp"
#include "Node.hpp"

static const unsigned MCTS_ITER_CHUNK = 50;

using namespace naivebot;
using namespace naivebot::mcts;

struct MCTS::MCTSImpl {
  unsigned timeoutMicroseconds;
  bool useEGreedy;

  uptr<Node> root;

  MCTSImpl(unsigned timeoutMicroseconds, bool useEGreedy)
      : timeoutMicroseconds(timeoutMicroseconds), useEGreedy(useEGreedy){};

  // TODO: handle the fact that the lifetime of an action returned here has to be less than or
  // equal to the lifetime of the MCTS.
  vector<ActionUtility> ComputeUtilities(const State &startState) {
    root = make_unique<Node>(startState, 0);

    Timer timer;
    timer.Start();

    unsigned totalIters = 0;
    while (timer.GetElapsedMicroseconds() < timeoutMicroseconds) {
      for (unsigned i = 0; i < MCTS_ITER_CHUNK; i++) {
        mcIteration(root.get());
        totalIters++;
      }
    }

    cout << "naive iters: " << totalIters << endl;
    timer.Stop();

    vector<ActionUtility> result = root->GetActionUtilities();
    sort(result.begin(), result.end(),
         [](const ActionUtility &a0, const ActionUtility &a1) { return a0.second > a1.second; });

    return result;
  }

  // single iteration of monte-carlo tree search.
  void mcIteration(Node *root) {
    vector<Node *> pathFromRoot;

    Node *cur = root;
    while (!cur->IsLeaf()) {
      pathFromRoot.push_back(cur);
      cur = cur->Select(useEGreedy);
    }
    pathFromRoot.push_back(cur);

    Node *playoutNode = cur->Expand();
    if (playoutNode == nullptr) {
      playoutNode = cur;
    } else {
      pathFromRoot.push_back(playoutNode);
    }

    double utility = playout(playoutNode);
    for (int i = pathFromRoot.size() - 1; i >= 0; i--) {
      pathFromRoot[i]->AddUtility(utility);
      utility = -utility;
    }
  }

  double playout(Node *startNode) {
    unsigned curPlayerIndex = startNode->PlayerIndex();
    State curState = startNode->GetState();

    vector<State> playedStates;
    while (!curState.IsTerminal()) {
      curState = randomSuccessor(curState);
      playedStates.push_back(curState);

      curPlayerIndex = 1 - curPlayerIndex;
    }

    // Account for the fact that the winner may not be the player of the original startNode.
    // The result of this function should be the utility of the playout for the player owning
    // the startNode.
    double utilFlip = startNode->PlayerIndex() == curPlayerIndex ? 1.0 : -1.0;

    if (curState.IsWin()) {
      return 1.0 * utilFlip;
    } else if (curState.IsLoss()) {
      return -1.0 * utilFlip;
    } else {
      return 0.0;
    }
  }

  State randomSuccessor(const State &state) {
    vector<Action> actions = state.AvailableActions();
    // We shouldnt be getting terminal states in this function.
    assert(!actions.empty());

    return state.SuccessorState(actions[rand() % actions.size()]);
  }
};

MCTS::MCTS(unsigned iters, bool useEGreedy) : impl(new MCTSImpl(iters, useEGreedy)) {}

MCTS::~MCTS() = default;

vector<ActionUtility> MCTS::ComputeUtilities(const State &curState) {
  return impl->ComputeUtilities(curState);
}
