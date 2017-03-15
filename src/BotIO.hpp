#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Bot.hpp"
#include "util/Common.hpp"

class BotIO {
public:
  BotIO(uptr<Bot> bot);
  ~BotIO() = default;

  void Loop(void);

private:
  uptr<Bot> bot;

  // static settings
  int timebank;
  int timePerMove;
  std::vector<std::string> playerNames;
  std::string myName;

  // dynamic settings
  int round;
  int move;
  std::string field;
  std::string macroboard;

  std::string line;
  std::vector<std::string> command;

  std::pair<int, int> action(const std::string &type, int time);
  void processCommand(const std::vector<std::string> &command);
  void update(const std::string &player, const std::string &type, const std::string &value);
  void setting(const std::string &type, const std::string &value);
};
