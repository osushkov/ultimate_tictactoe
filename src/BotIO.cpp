
#include "Bot.hpp"

#include <cstdlib>
#include <string>
#include <vector>

BotIO::BotIO() {
  _field.resize(81);
  _macroboard.resize(9);
}

void loop() {
  std::string line;
  std::vector<std::string> command;
  command.reserve(256);

  while (std::getline(std::cin, line)) {
    processCommand(split(line, ' ', command));
  }
}

private:
/**
 * Implement this function.
 * type is always "move"
 *
 * return value must be position in x,y presentation
 *      (use std::make_pair(x, y))
 */
std::pair<int, int> action(const std::string &type, int time) { return getRandomFreeCell(); }

/**
 * Returns random free cell.
 * It can be used to make your bot more immune to errors
 * Use next pattern in action method:
 *
 *      try{
 *          ... YOUR ALGORITHM ...
 *      }
 *      catch(...) {
 *          return getRandomCell();
 *      }
 *
 */
std::pair<int, int> getRandomFreeCell() const {
  debug("Using random algorithm.");
  std::vector<int> freeCells;
  for (int i = 0; i < 81; ++i) {
    int blockId = ((i / 27) * 3) + (i % 9) / 3;
    if (_macroboard[blockId] == -1 && _field[i] == 0) {
      freeCells.push_back(i);
    }
  }
  int randomCell = freeCells[rand() % freeCells.size()];
  return std::make_pair(randomCell % 9, randomCell / 9);
}

void processCommand(const std::vector<std::string> &command) {
  if (command[0] == "action") {
    auto point = action(command[1], stringToInt(command[2]));
    std::cout << "place_move " << point.first << " " << point.second << std::endl << std::flush;
  } else if (command[0] == "update") {
    update(command[1], command[2], command[3]);
  } else if (command[0] == "settings") {
    setting(command[1], command[2]);
  } else {
    debug("Unknown command <" + command[0] + ">.");
  }
}

void update(const std::string &player, const std::string &type, const std::string &value) {
  if (player != "game" && player != _myName) {
    // It's not my update!
    return;
  }

  if (type == "round") {
    _round = stringToInt(value);
  } else if (type == "move") {
    _move = stringToInt(value);
  } else if (type == "macroboard" || type == "field") {
    std::vector<std::string> rawValues;
    split(value, ',', rawValues);
    std::vector<int>::iterator choice = (type == "field" ? _field.begin() : _macroboard.begin());
    std::transform(rawValues.begin(), rawValues.end(), choice, stringToInt);
  } else {
    debug("Unknown update <" + type + ">.");
  }
}

void setting(const std::string &type, const std::string &value) {
  if (type == "timebank") {
    _timebank = stringToInt(value);
  } else if (type == "time_per_move") {
    _timePerMove = stringToInt(value);
  } else if (type == "player_names") {
    split(value, ',', _playerNames);
  } else if (type == "your_bot") {
    _myName = value;
  } else if (type == "your_botid") {
    _botId = stringToInt(value);
  } else {
    debug("Unknown setting <" + type + ">.");
  }
}

void debug(const std::string &s) const { std::cerr << s << std::endl << std::flush; }

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
}
;
