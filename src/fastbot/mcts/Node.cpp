
#include "Node.hpp"
#include "../../util/Util.hpp"
#include "../State.hpp"
#include <cassert>
#include <iostream>

static constexpr float P_RANDOM = 0.2f;
static constexpr float UCB1c = 0.1f; // sqrtf(2.0f);

using namespace fastbot;
using namespace fastbot::mcts;

Node::Node(const State &state, unsigned char playerIndex, unsigned depth)
    : state(state), nonExpandedActions(state.AvailableActions()), playerIndex(playerIndex),
      depth(depth), isLeaf(true), totalTrials(0), sumUtility(0.0f) {}

bool Node::IsLeaf(void) const { return isLeaf; }

unsigned char Node::PlayerIndex(void) const { return playerIndex; }

State Node::GetState(void) const { return state; }

vector<pair<Action, float>> Node::GetActionUtilities(void) const {
  vector<pair<Action, float>> result;
  for (const auto &edge : children) {
    result.emplace_back(edge.first, edge.second->ExpectedUtility(playerIndex));
  }
  return result;
}

Node *Node::Expand(void) {
  assert(isLeaf);

  if (nonExpandedActions.empty()) {
    return nullptr;
  } else if (nonExpandedActions.size() == 1) {
    isLeaf = false;
  }

  assert(!state.IsTerminal());

  unsigned chosenIndex = rand() % nonExpandedActions.size();
  auto chosenAction = nonExpandedActions[chosenIndex];

  nonExpandedActions[chosenIndex] = nonExpandedActions.back();
  nonExpandedActions.pop_back();

  State childState = state.SuccessorState(chosenAction);
  children.emplace_back(chosenAction, make_unique<Node>(childState, 1 - playerIndex, depth + 1));

  return children.back().second.get();
}

Node *Node::Select(const Spec &spec) {
  assert(!isLeaf);
  assert(!children.empty());

  // if (useEGreedy) {
  //   return eGreedySelect();
  // } else {
  return UCB1Select(spec);
  // }
}

void Node::AddUtility(float utility) {
  totalTrials++;
  sumUtility += utility;
}

float Node::ExpectedUtility(unsigned char playerIndex) const {
  assert(totalTrials > 0);

  float p = sumUtility / totalTrials;
  if (this->playerIndex == playerIndex) {
    return p;
  } else {
    return -p;
  }
}

Node *Node::eGreedySelect(void) {
  bool chooseRandom = Util::RandInterval(0.0, 1.0) < P_RANDOM;
  if (chooseRandom) {
    return children[rand() % children.size()].second.get();
  } else {
    Node *result = nullptr;
    double bestUtility = 0.0;

    for (auto &edge : children) {
      double utility = edge.second->ExpectedUtility(this->playerIndex);
      if (result == nullptr || utility > bestUtility) {
        bestUtility = utility;
        result = edge.second.get();
      }
    }

    assert(result != nullptr);
    return result;
  }
}

Node *Node::UCB1Select(const Spec &spec) {
  if (Util::RandInterval(0.0, 1.0) < spec.pRandomSelect) {
    return children[rand() % children.size()].second.get();
  }

  Node *result = nullptr;
  float bestUtility = 0.0f;

  for (auto &edge : children) {
    Node *cn = edge.second.get();

    float eu = cn->ExpectedUtility(this->playerIndex);
    float eb = sqrtf(log(static_cast<float>(totalTrials)) / static_cast<float>(cn->totalTrials));
    float ebw = spec.explorationWeightC + depth * spec.explorationWeightA;
    float rw = Util::RandInterval(0.0f, spec.randomWeight);
    float w = eu + (eb * ebw) + rw;

    if (result == nullptr || w > bestUtility) {
      bestUtility = w;
      result = cn;
    }
  }

  return result;
}
