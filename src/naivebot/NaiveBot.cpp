
#include "NaiveBot.hpp"
#include "mcts/MCTS.hpp"
#include <sstream>
#include <cassert>

using namespace naivebot;

struct NaiveBot::NaiveBotImpl {
  int botId;

  NaiveBotImpl() : botId(1) {};

  void SetBotId(int botId) {
    assert(botId == 1 || botId == 2);
    this->botId = botId;
  };

  pair<int, int> ChooseAction(const string &field) {
    mcts::MCTS mcts(10000);
    vector<mcts::ActionUtility> actions = mcts.ComputeUtilities(parseState(field));
    return make_pair(actions.front().first.x, actions.front().first.y);
  }

  State parseState(const string &field) {
    std::vector<std::string> rawValues;
    split(field, ',', rawValues);
    assert(rawValues.size() == NUM_CELLS);

    array<CellState, NUM_CELLS> fieldCells;
    for (unsigned i = 0; i < rawValues.size(); i++) {
      int v = stringToInt(rawValues[i]);
      assert(v >= 0 && v <= 2);

      fieldCells[i] = v == 0 ? CellState::EMPTY :
          (v == botId ? CellState::MY_TOKEN : CellState::OPPONENT_TOKEN);
    }

    return State(fieldCells);
  }

  std::vector<std::string> &split(
          const std::string &s, char delim, std::vector<std::string> &elems) {
      std::stringstream ss(s);
      std::string item;
      elems.clear();
      while (std::getline(ss, item, delim)) {
          elems.push_back(item);
      }
      return elems;
  }

  int stringToInt(const std::string &s) {
      std::istringstream ss(s);
      int result;
      ss >> result;
      return result;
  }
};

NaiveBot::NaiveBot() : impl(new NaiveBotImpl()) {}
NaiveBot::~NaiveBot() = default;

void NaiveBot::SetBotId(int botId) { impl->SetBotId(botId); }

pair<int, int> NaiveBot::ChooseAction(const string &field) { return impl->ChooseAction(field); }
