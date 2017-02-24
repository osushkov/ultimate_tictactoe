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

  vector<float> RunTournament(const function<uptr<naivebot::NaiveBot>()> &naivebotBuilder,
                              const function<uptr<fastbot::FastBot>()> &fastbotBuilder,
                              unsigned rounds);

private:
  struct TournamentImpl;
  uptr<TournamentImpl> impl;
};
