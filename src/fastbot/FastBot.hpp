
#pragma once

#include "Action.hpp"
#include "State.hpp"
#include "../util/Common.hpp"
#include "../Bot.hpp"

namespace fastbot {

class FastBot : public Bot {
public:
  FastBot(unsigned microsecondsPerMove, bool useEGreedy);
  ~FastBot();

  void SetBotId(int botId) override;
  pair<int, int> ChooseAction(const string &field) override;

  Action ChooseAction(const State &state);

private:
  struct FastBotImpl;
  uptr<FastBotImpl> impl;
};
}
