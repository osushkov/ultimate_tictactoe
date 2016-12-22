
#pragma once

#include "../util/Common.hpp"
#include "../Bot.hpp"

namespace naivebot {

class NaiveBot : public Bot {
public:
  NaiveBot();
  ~NaiveBot();

  pair<int, int> ChooseAction(const string &field) override;

private:
  struct NaiveBotImpl;
  uptr<NaiveBotImpl> impl;
};
}
