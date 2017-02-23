
#pragma once

#include "../util/Common.hpp"
#include <ostream>

namespace fastbot {

class Action {
public:
  unsigned x, y;

  Action(unsigned x, unsigned y);
  ~Action() = default;

  bool operator==(const Action &other) const;

  // Returns a hash of this action.
  size_t HashCode(void) const;

  void Output(std::ostream &out) const;
};
}

namespace std {
template <> struct hash<fastbot::Action *> {
  inline size_t operator()(naivebot::Action const *action) const { return action->HashCode(); }
};
}
