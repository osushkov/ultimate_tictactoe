#pragma once

#include <functional>
#include <vector>

#include "Bot.hpp"
#include "fastbot/FastBot.hpp"
#include "util/Common.hpp"

class Tournament {
public:
  Tournament();
  ~Tournament();

  vector<float> RunTournament(const vector<function<uptr<fastbot::FastBot>()>> &fastbotBuilder,
                              unsigned rounds);

private:
  struct TournamentImpl;
  uptr<TournamentImpl> impl;
};
