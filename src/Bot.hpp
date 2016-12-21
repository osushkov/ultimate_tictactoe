
#pragma once

#include "util/Common.hpp"
#include <string>

class Bot {
public:
  virtual pair<int, int> ChooseAction();
};
