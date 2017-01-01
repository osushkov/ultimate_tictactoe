
#include "Node.hpp"
#include "../../util/Util.hpp"
#include "../State.hpp"
#include <cassert>

using namespace naivebot;
using namespace naivebot::mcts;

Node::Node(const State &state, unsigned playerIndex)
    : state(state), playerIndex(playerIndex), isLeaf(true), totalTrials(0), sumUtility(0.0) {}

bool Node::IsLeaf(void) const { return isLeaf; }

unsigned Node::PlayerIndex(void) const { return playerIndex; }

State Node::GetState(void) const { return state; }

vector<pair<Action, double>> Node::GetActionUtilities(void) const {
  vector<pair<Action, double>> result;
  for (const auto &edge : children) {
    result.push_back(make_pair(edge.first, edge.second->ExpectedUtility(playerIndex)));
  }
  return result;
}

Node *Node::Expand(void) {
  assert(isLeaf);

  vector<Action> available = nonExpandedActions();
  if (available.empty()) { // This can happen at a terminal game state.
    return nullptr;
  }

  if (available.size() == 1) {
    isLeaf = false;
  }

  auto &chosenAction = available[rand() % available.size()];
  State childState = state.SuccessorState(chosenAction);

  children.emplace_back(chosenAction, make_unique<Node>(childState, 1 - playerIndex));
  return children.back().second.get();
}

Node *Node::Select(double pRandom) {
  assert(!isLeaf);
  assert(!children.empty());

  bool chooseRandom = Util::RandInterval(0.0, 1.0) < pRandom;
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

void Node::AddUtility(double utility) {
  totalTrials++;
  sumUtility += utility;
}

// This is just a quick and dirty hack, can be more efficient but on small branch factor
// problems doesnt really make a difference.
vector<Action> Node::nonExpandedActions(void) {
  vector<Action> stateActions = state.AvailableActions();

  vector<Action> result;
  for (auto &sa : stateActions) {
    bool shouldAdd = true;
    for (auto &c : children) {
      if (sa == c.first) {
        shouldAdd = false;
        break;
      }
    }
    if (shouldAdd) {
      result.push_back(sa);
    }
  }
  return result;
}

double Node::ExpectedUtility(unsigned playerIndex) const {
  assert(totalTrials > 0);

  double p = sumUtility / totalTrials;
  if (this->playerIndex == playerIndex) {
    return p;
  } else {
    return -p;
  }
}
