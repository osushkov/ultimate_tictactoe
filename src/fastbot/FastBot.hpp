
#pragma once

#include "../Bot.hpp"
#include "../util/Common.hpp"
#include "Action.hpp"
#include "Spec.hpp"
#include "State.hpp"

namespace fastbot {

class FastBot : public Bot {
public:
  FastBot(const Spec &spec);
  ~FastBot();

  void SetBotId(unsigned char botId) override;
  unsigned char GetBotId(void) const override;

  void SetTimePerMove(unsigned milliseconds) override;
  void SetTimeBank(unsigned milliseconds) override;

  pair<int, int> ChooseAction(const string &field, signed char topCellRestriction) override;
  Action ChooseAction(const State &state);

private:
  struct FastBotImpl;
  uptr<FastBotImpl> impl;
};
}
