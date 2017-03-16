
#include "MCTS.hpp"
#include "../../util/Timer.hpp"
#include "Node.hpp"

static const unsigned MCTS_ITER_CHUNK = 50;

using namespace fastbot;
using namespace fastbot::mcts;

static unsigned totalIters = 0;

struct MCTS::MCTSImpl {
  Spec spec;
  uptr<Node> root;

  MCTSImpl(const Spec &spec) : spec(spec), root(nullptr){};

  // TODO: handle the fact that the lifetime of an action returned here has to be less than or
  // equal to the lifetime of the MCTS.
  vector<ActionUtility> ComputeUtilities(const State &startState, unsigned timeoutMilliseconds) {
    if (!tryReuseSubtree(startState)) {
      root = make_unique<Node>(startState, 0, 0);
    }

    Timer timer;
    timer.Start();

    const unsigned timeoutMicroseconds = (timeoutMilliseconds * 1000);
    while (timer.GetElapsedMicroseconds() < timeoutMicroseconds) {
      for (unsigned i = 0; i < MCTS_ITER_CHUNK; i++) {
        mcIteration(root.get());
        totalIters++;
      }
    }

    // cout << "fast iters: " << totalIters << " " << timeoutMilliseconds << endl;
    timer.Stop();

    vector<ActionUtility> result = root->GetActionUtilities();
    sort(result.begin(), result.end(),
         [](const ActionUtility &a0, const ActionUtility &a1) { return a0.second > a1.second; });

    return result;
  }

  bool tryReuseSubtree(const State &startState) {
    // return false;
    if (!root) {
      return false;
    }

    if (root->GetState() == startState) {
      return true;
    }

    for (auto &edge : root->GetChildren()) {
      for (auto &edge2 : edge.second->GetChildren()) {
        if (edge2.second->GetState() == startState) {
          uptr<Node> newRoot = move(edge2.second);
          root = move(newRoot);
          return true;
        }
      }
    }

    return false;
  }

  // single iteration of monte-carlo tree search.
  void mcIteration(Node *root) {
    vector<Node *> pathFromRoot;

    Node *cur = root;
    while (!cur->IsLeaf()) {
      pathFromRoot.push_back(cur);
      cur = cur->Select(spec);
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

    while (!curState.IsTerminal()) {
      curState = randomSuccessor(curState);
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
    return state.SuccessorState(state.ChooseRandomAction());
  }
};

MCTS::MCTS(const Spec &spec) : impl(new MCTSImpl(spec)) {}

MCTS::~MCTS() = default;

vector<ActionUtility> MCTS::ComputeUtilities(const State &curState, unsigned timeoutMilliseconds) {
  return impl->ComputeUtilities(curState, timeoutMilliseconds);
}
