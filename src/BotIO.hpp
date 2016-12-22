#pragma once

#include <vector>
#include <string>
#include <utility>

class BotIO {
public:
  BotIO();
  ~BotIO() = default;
  
  void Loop(void);

private:
  // static settings
  int _timebank;
  int _timePerMove;
  int _botId;
  std::vector<std::string> _playerNames;
  std::string _myName;

  // dynamic settings
  int _round;
  int _move;
  std::vector<int> _macroboard;
  std::vector<int> _field;

  std::pair<int, int> action(const std::string &type, int time);
  std::pair<int, int> getRandomFreeCell() const;
  void processCommand(const std::vector<std::string> &command);
  void update(const std::string &player, const std::string &type, const std::string &value);
  void setting(const std::string &type, const std::string &value);
};
