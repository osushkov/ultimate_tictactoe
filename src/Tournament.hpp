#pragma once

#include <vector>
#include <functional>

#include "naivebot/NaiveBot.hpp"
#include "fastbot/FastBot.hpp"
#include "util/Common.hpp"
#include "Bot.hpp"

class Tournament {
public:
  Tournament();
  ~Tournament();

  vector<float> RunTournament(const vector<function<uptr<naivebot::NaiveBot>()>> &botBuilders,
                              unsigned rounds);

private:
  struct TournamentImpl;
  uptr<TournamentImpl> impl;
};
