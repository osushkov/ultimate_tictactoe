#pragma once

#include <functional>
#include <vector>

#include "Bot.hpp"
#include "fastbot/FastBot.hpp"
#include "naivebot/NaiveBot.hpp"
#include "util/Common.hpp"

class Tournament {
public:
  Tournament();
  ~Tournament();

  vector<float> RunTournament(const vector<function<uptr<fastbot::FastBot>()>> &fastbotBuilder,
                              unsigned rounds);

  vector<float> RunTournament(const function<uptr<naivebot::NaiveBot>()> &naivebotBuilder,
                              const function<uptr<fastbot::FastBot>()> &fastbotBuilder,
                              unsigned rounds);

private:
  struct TournamentImpl;
  uptr<TournamentImpl> impl;
};
