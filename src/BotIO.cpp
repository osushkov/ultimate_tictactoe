
#include "BotIO.hpp"

#include <cstdlib>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <time.h>

using namespace std;

std::vector<std::string> &split(
        const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    elems.clear();
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


int stringToInt(const std::string &s) {
    std::istringstream ss(s);
    int result;
    ss >> result;
    return result;
}

void debug(const std::string &s) {
  std::cerr << s << std::endl << std::flush;
}

BotIO::BotIO() {
  _field.resize(81);
  _macroboard.resize(9);
}

void BotIO::Loop() {
  std::string line;
  std::vector<std::string> command;
  command.reserve(256);

  while (std::getline(std::cin, line)) {
    processCommand(split(line, ' ', command));
  }
}

std::pair<int, int> BotIO::action(const std::string &type, int time) {
  return getRandomFreeCell();
}

std::pair<int, int> BotIO::getRandomFreeCell() const {
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

void BotIO::processCommand(const std::vector<std::string> &command) {
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

void BotIO::update(const std::string &player, const std::string &type, const std::string &value) {
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

void BotIO::setting(const std::string &type, const std::string &value) {
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
