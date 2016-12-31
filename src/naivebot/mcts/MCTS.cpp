
#include "MCTS.hpp"
#include "../Rules.hpp"
#include "Node.hpp"

static const double P_RANDOM = 0.2;
static const unsigned MC_ITER = 100000;

using namespace naivebot;
using namespace naivebot::mcts;

struct MCTS::MCTSImpl {
  uptr<Node> root;

  MCTSImpl() = default;

  // TODO: handle the fact that the lifetime of an action returned here has to be less than or
  // equal to the lifetime of the MCTS.
  vector<ActionUtility> ComputeUtilities(const State &startState) {
    root = make_unique<Node>(startState, 0);

    for (unsigned i = 0; i < MC_ITER; i++) {
      mcIteration(root.get());
    }

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
      cur = cur->Select(P_RANDOM);
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
    Rules *rules = Rules::Instance();

    unsigned curPlayerIndex = startNode->PlayerIndex();
    State curState = startNode->GetState();

    vector<State> playedStates;
    while (!rules->IsTerminalState(curState)) {
      curState = randomSuccessor(curState);
      playedStates.push_back(curState);

      curPlayerIndex = 1 - curPlayerIndex;
    }

    // Account for the fact that the winner may not be the player of the original startNode.
    // The result of this function should be the utility of the playout for the player owning
    // the startNode.
    double utilFlip = startNode->PlayerIndex() == curPlayerIndex ? 1.0 : -1.0;

    if (rules->IsWin(curState)) {
      return 1.0 * utilFlip;
    } else if (rules->IsLoss(curState)) {
      return -1.0 * utilFlip;
    } else {
      return 0.0;
    }
  }

  State randomSuccessor(const State &state) {
    vector<Action> actions = state.AvailableActions();
    // We shouldnt be getting terminal states in this function.
    assert(!actions.empty());

    State result = state.SuccessorState(actions[rand() % actions.size()]);

    // TODO: this is a bit hacky, fix this here and other places.
    result.FlipState();
    return result;
  }
};

MCTS::MCTS() : impl(new MCTSImpl()) {}

MCTS::~MCTS() = default;

vector<ActionUtility> MCTS::ComputeUtilities(const State &curState) {
  return impl->ComputeUtilities(curState);
}
