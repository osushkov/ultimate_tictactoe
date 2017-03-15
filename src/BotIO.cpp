
#include "BotIO.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <utility>
#include <vector>

using namespace std;

static std::vector<std::string> &split(const std::string &s, char delim,
                                       std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  elems.clear();
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

static int stringToInt(const std::string &s) {
  std::istringstream ss(s);
  int result;
  ss >> result;
  return result;
}

static void debug(const std::string &s) { std::cerr << s << std::endl << std::flush; }

BotIO::BotIO(uptr<Bot> bot) : bot(std::move(bot)) { command.reserve(256); }

void BotIO::Loop() {
  while (std::getline(std::cin, line)) {
    processCommand(split(line, ' ', command));
  }
}

std::pair<int, int> BotIO::action(const std::string &type, int time) {
  std::vector<std::string> macroVals;
  macroVals = split(macroboard, ',', macroVals);

  signed char topCellRestriction = -1;
  bool haveSeenMinusOne = false;

  for (unsigned ci = 0; ci < macroVals.size(); ci++) {
    int iv = stringToInt(macroVals[ci]);
    if (haveSeenMinusOne && iv == -1) {
      topCellRestriction = -1;
    } else if (iv == -1) {
      haveSeenMinusOne = true;
      topCellRestriction = static_cast<signed char>(ci);
    }
  }

  bot->SetTimeBank(static_cast<unsigned>(time));
  return bot->ChooseAction(field, topCellRestriction);
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
  if (player != "game" && player != myName) {
    // It's not my update!
    return;
  }

  if (type == "round") {
    round = stringToInt(value);
  } else if (type == "move") {
    move = stringToInt(value);
  } else if (type == "field") {
    field = value;
  } else if (type == "macroboard") {
    macroboard = value;
  } else {
    debug("Unknown update <" + type + ">.");
  }
}

void BotIO::setting(const std::string &type, const std::string &value) {
  if (type == "timebank") {
    timebank = stringToInt(value);
    if (timebank >= 0) {
      bot->SetTimeBank(static_cast<unsigned>(timebank));
    }
  } else if (type == "time_per_move") {
    timePerMove = stringToInt(value);
    if (timePerMove >= 0) {
      bot->SetTimePerMove(static_cast<unsigned>(timePerMove));
    }
  } else if (type == "player_names") {
    split(value, ',', playerNames);
  } else if (type == "your_bot") {
    myName = value;
  } else if (type == "your_botid") {
    bot->SetBotId(stringToInt(value));
  } else {
    debug("Unknown setting <" + type + ">.");
  }
}
