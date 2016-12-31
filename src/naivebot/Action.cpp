
#include "Action.hpp"
#include <cassert>

using namespace naivebot;

Action::Action(unsigned x, unsigned y) : x(x), y(y) {
  assert(x < 9);
  assert(y < 9);
}

bool Action::operator==(const Action &other) const { return x == other.x && y == other.y; }

// Returns a hash of this action.
size_t Action::HashCode(void) const { return x + y * 9; }

void Action::Output(std::ostream &out) const { out << "action( " << x << "," << y << " )"; }
