#pragma once

#include "Action.hpp"
#include "State.hpp"

namespace fastbot {
namespace openingmoves {

bool HaveActionFor(const State &state);
Action GetActionFor(const State &state);
}
}
