
#pragma once

#include "Action.hpp"
#include "State.hpp"
#include "../util/Common.hpp"
#include "../Bot.hpp"

namespace naivebot {

class NaiveBot : public Bot {
public:
  NaiveBot(unsigned microsecondsPerMove, bool useEGreedy);
  ~NaiveBot();

  void SetBotId(int botId) override;
  pair<int, int> ChooseAction(const string &field) override;

  Action ChooseAction(const State &state);

private:
  struct NaiveBotImpl;
  uptr<NaiveBotImpl> impl;
};
}
