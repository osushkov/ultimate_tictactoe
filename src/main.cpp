
#include <cstdlib>
#include <iostream>
#include "Tournament.hpp"
#include "BotIO.hpp"
#include "naivebot/NaiveBot.hpp"

using namespace std;

int main() {
  srand(time(NULL));

  cout << "hello world" << endl;

  auto bot = make_unique<naivebot::NaiveBot>();

  BotIO botIO(std::move(bot));
  botIO.Loop();

  return 0;
}
