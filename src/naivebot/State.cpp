
#include "State.hpp"
#include <algorithm>
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

std::ostream &operator<<(std::ostream &stream, const TopCellState &tcs) {
  switch (tcs) {
  case TopCellState::UNDECIDED:
    stream << ".";
    break;
  case TopCellState::DRAW:
    stream << "D";
    break;
  case TopCellState::MY_TOKEN:
    stream << "X";
    break;
  case TopCellState::OPPONENT_TOKEN:
    stream << "O";
    break;
  }
  return stream;
}

static TopCellState calculateGridState(const array<CellState, NUM_CELLS> &grid, unsigned startX,
                                       unsigned startY) {

  return TopCellState::UNDECIDED;
}

static TopCellState calculateGridState(const array<TopCellState, NUM_TOP_CELLS> &grid) {
  unsigned numUnfilled = 0;
  unsigned numNeutral = 0;
  for (unsigned i = 0; i < NUM_TOP_CELLS; i++) {
    if (grid[i] == TopCellState::UNDECIDED) {
      numNeutral++;
      numUnfilled++;
    } else if (grid[i] == TopCellState::DRAW) {
      numNeutral++;
    }
  }

  if (numNeutral > 4) {
    return numUnfilled == 0 ? TopCellState::DRAW : TopCellState::UNDECIDED;
  }

  return TopCellState::UNDECIDED;
}

State::State() : isTerminal(false), isWin(false), isLoss(false) {
  cells.fill(CellState::EMPTY);
  topCells.fill(TopCellState::UNDECIDED);
}

State::State(const State &other)
    : cells(other.cells), topCells(other.topCells), isTerminal(other.isTerminal),
      isWin(other.isWin), isLoss(other.isLoss) {}

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

  for (unsigned ty = 0; ty < 3; ty++) {
    for (unsigned tx = 0; tx < 3; tx++) {
      if (topCells[tx + ty * 3] == TopCellState::UNDECIDED) {
        unsigned sy = ty * 3;
        unsigned ey = sy + 3;
        unsigned sx = tx * 3;
        unsigned ex = sx + 3;

        for (unsigned y = sy; y < ey; y++) {
          for (unsigned x = sx; x < ex; x++) {
            if (cells[x + y * 9] == CellState::EMPTY) {
              result.emplace_back(x, y);
            }
          }
        }
      }
    }
  }

  return result;
}

State State::SuccessorState(const Action &action) const {
  State result(*this);
  assert(!result.isTerminal && !result.isWin && !result.isLoss);

  unsigned topX = action.x / 3;
  unsigned topY = action.y / 3;
  assert(result.topCells[topX + topY * 3] == TopCellState::UNDECIDED);

  assert(result.cells[action.x + action.y * 9] == CellState::EMPTY);
  result.cells[action.x + action.y * 9] = CellState::MY_TOKEN;

  auto newTopState = calculateGridState(result.cells, topX * 3, topY * 3);
  result.topCells[topX + topY * 3] = newTopState;

  if (newTopState != TopCellState::UNDECIDED) {
    auto newGameState = calculateGridState(result.topCells);

    switch (newGameState) {
    case TopCellState::DRAW:
      result.isTerminal = true;
      break;
    case TopCellState::MY_TOKEN:
      result.isTerminal = true;
      result.isWin = true;
      break;
    case TopCellState::OPPONENT_TOKEN:
      assert(false); // This should never happen.
      break;
    case TopCellState::UNDECIDED:
      break;
    }
  }

  return result;
}

bool State::IsTerminal(void) const { return isTerminal; }
bool State::IsWin(void) const { return isWin; }
bool State::IsLoss(void) const { return isLoss; }

void State::flipState(void) {
  for (auto &cell : cells) {
    if (cell == CellState::MY_TOKEN) {
      cell = CellState::OPPONENT_TOKEN;
    } else if (cell == CellState::OPPONENT_TOKEN) {
      cell = CellState::MY_TOKEN;
    }
  }

  for (auto &cell : topCells) {
    if (cell == TopCellState::MY_TOKEN) {
      cell = TopCellState::OPPONENT_TOKEN;
    } else if (cell == TopCellState::OPPONENT_TOKEN) {
      cell = TopCellState::MY_TOKEN;
    }
  }

  swap(isWin, isLoss);
}
