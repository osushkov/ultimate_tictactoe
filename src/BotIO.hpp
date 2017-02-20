#pragma once

#include <vector>
#include <string>
#include <utility>

#include "util/Common.hpp"
#include "Bot.hpp"

class BotIO {
public:
  BotIO(uptr<Bot> bot);
  ~BotIO() = default;

  void Loop(void);

private:
  uptr<Bot> bot;

  // static settings
  int _timebank;
  int _timePerMove;
  std::vector<std::string> _playerNames;
  std::string _myName;

  // dynamic settings
  int _round;
  int _move;
  std::string field;

  std::string line;
  std::vector<std::string> command;

  std::pair<int, int> action(const std::string &type, int time);
  void processCommand(const std::vector<std::string> &command);
  void update(const std::string &player, const std::string &type, const std::string &value);
  void setting(const std::string &type, const std::string &value);
};
