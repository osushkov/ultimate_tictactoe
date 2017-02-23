
#include "Node.hpp"
#include "../../util/Util.hpp"
#include "../State.hpp"
#include <cassert>
#include <iostream>

static constexpr float P_RANDOM = 0.2f;
static constexpr float UCB1c = 0.1f;//sqrtf(2.0f);

using namespace fastbot;
using namespace fastbot::mcts;

Node::Node(const State &state, unsigned char playerIndex)
    : state(state), playerIndex(playerIndex), isLeaf(true), totalTrials(0), sumUtility(0.0f) {}

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

  vector<Action> available = nonExpandedActions();
  if (available.empty()) { // This can happen at a terminal game state.
    return nullptr;
  }

  if (available.size() == 1) {
    isLeaf = false;
  }

  assert(!state.IsTerminal());
  auto &chosenAction = available[rand() % available.size()];
  State childState = state.SuccessorState(chosenAction);

  children.emplace_back(chosenAction, make_unique<Node>(childState, 1 - playerIndex));
  return children.back().second.get();
}

Node *Node::Select(bool useEGreedy) {
  assert(!isLeaf);
  assert(!children.empty());

  if (useEGreedy) {
   return eGreedySelect();
  } else {
    return UCB1Select();
  }
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

Node* Node::eGreedySelect(void) {
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

Node* Node::UCB1Select(void) {
    if (Util::RandInterval(0.0, 1.0) < 0.3f) {
      return children[rand() % children.size()].second.get();
    }

    Node *result = nullptr;
    double bestUtility = 0.0;

    for (auto &edge : children) {
        Node* cn = edge.second.get();

        float eu = cn->ExpectedUtility(this->playerIndex);
        float eb = UCB1c * sqrtf(log(static_cast<float>(totalTrials)) / static_cast<float>(cn->totalTrials));
        // float rw = Util::RandInterval(0.0f, 1.0f);
        float w = eu + eb;// + rw;

        // cout << eu << " " << eb << endl;

        if (result == nullptr || w > bestUtility) {
            bestUtility = w;
            result = cn;
        }
    }

    return result;
}
