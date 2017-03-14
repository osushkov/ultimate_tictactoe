
#pragma once

#include "util/Common.hpp"
#include <string>

class Bot {
public:
  // botId must be either 1 or 2.
  virtual void SetBotId(unsigned char botId) = 0;
  virtual unsigned char GetBotId(void) const = 0;

  virtual void SetTimePerMove(unsigned milliseconds) = 0;
  virtual void SetTimeBank(unsigned milliseconds) = 0;

  virtual pair<int, int> ChooseAction(const string &field, signed char topCellRestriction) = 0;
};
