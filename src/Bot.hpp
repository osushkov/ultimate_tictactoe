
#pragma once

#include "util/Common.hpp"
#include <string>

class Bot {
public:

  virtual void SetBotId(int botId) = 0;
  // virtual void SetTimeRemaining(unsigned milliseconds);

  virtual pair<int, int> ChooseAction(const string &field) = 0;
};
