
#include "State.hpp"
#include <cassert>
#include <iostream>

using namespace naivebot;

std::ostream &operator<<(std::ostream &stream, const CellState &cs) {
  switch (cs) {
  case CellState::MY_TOKEN:
    stream << "X";
    break;
  case CellState::OPPONENT_TOKEN:
    stream << "O";
    break;
  case CellState::EMPTY:
    stream << ".";
    break;
  }
  return stream;
}

State::State() {
  cells.fill(CellState::EMPTY);
  topCells.fill(CellState::EMPTY);
}

State::State(const State &other) : cells(other.cells), topCells(other.topCells) {}

bool State::operator==(const State &other) const {
  assert(cells.size() == other.cells.size());
  for (unsigned i = 0; i < cells.size(); i++) {
    if (cells[i] != other.cells[i]) {
      return false;
    }
  }

  return true;
}

size_t State::HashCode() const {
  size_t result = 0;
  for (const auto &c : cells) {
    result = 3 * result + (c == CellState::EMPTY ? 0 : (c == CellState::MY_TOKEN ? 1 : 2));
  }
  return result;
}

void State::Output(std::ostream &out) const {
  out << "top level:" << endl;
  for (unsigned y = 0; y < 3; y++) {
    for (unsigned x = 0; x < 3; x++) {
      out << topCells[x + y * 3];
    }
    out << endl;
  }

  out << "raw cells:" << endl;
  for (unsigned y = 0; y < 9; y++) {
    for (unsigned x = 0; x < 9; x++) {
      out << cells[x + y * 9];
    }
    out << endl;
  }
}

vector<Action> State::AvailableActions(void) const {
  vector<Action> result;
  return result;
}

State State::SuccessorState(const Action &action) const {
  State result(*this);

  unsigned topX = action.x / 3;
  unsigned topY = action.y / 3;
  assert(result.topCells[topX + topY * 3] == CellState::EMPTY);

  assert(result.cells[action.x + action.y * 9] == CellState::EMPTY);
  result.cells[action.x + action.y * 9] = CellState::MY_TOKEN;

  return result;
}

void State::FlipState(void) {
  for (auto &cell : cells) {
    if (cell == CellState::MY_TOKEN) {
      cell = CellState::OPPONENT_TOKEN;
    } else if (cell == CellState::OPPONENT_TOKEN) {
      cell = CellState::MY_TOKEN;
    }
  }
}
