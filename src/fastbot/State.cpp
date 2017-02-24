
#include "State.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

using namespace fastbot;

std::ostream &fastbot::operator<<(std::ostream &stream, const CellState &cs) {
  switch (cs) {
  case CellState::CROSS:
    stream << "X";
    break;
  case CellState::NAUGHT:
    stream << "O";
    break;
  case CellState::EMPTY:
    stream << ".";
    break;
  }
  return stream;
}

std::ostream &fastbot::operator<<(std::ostream &stream, const TopCellState &tcs) {
  switch (tcs) {
  case TopCellState::UNDECIDED:
    stream << ".";
    break;
  case TopCellState::DRAW:
    stream << "D";
    break;
  case TopCellState::CROSS:
    stream << "X";
    break;
  case TopCellState::NAUGHT:
    stream << "O";
    break;
  }
  return stream;
}

static array<array<unsigned char, 3>, 8> runIndices{{array<unsigned char, 3>{{0, 1, 2}}, array<unsigned char, 3>{{3, 4, 5}},
                                                     array<unsigned char, 3>{{6, 7, 8}}, array<unsigned char, 3>{{0, 3, 6}},
                                                     array<unsigned char, 3>{{1, 4, 7}}, array<unsigned char, 3>{{2, 5, 8}},
                                                     array<unsigned char, 3>{{0, 4, 8}}, array<unsigned char, 3>{{2, 4, 6}}}};

static unsigned char findRun(const array<unsigned char, NUM_TOP_CELLS> &grid) {
  for (unsigned i = 0; i < runIndices.size(); i++) {
    unsigned char val = grid[runIndices[i][0]];
    if (val == 0) {
      continue;
    }

    bool allSame = true;
    for (unsigned j = 1; j < runIndices[i].size(); j++) {
      if (val != grid[runIndices[i][j]]) {
        allSame = false;
        break;
      }
    }

    if (allSame) {
      return val;
    }
  }

  return 0;
}

static TopCellState calculateGridState(const array<CellState, NUM_CELLS> &grid,
                                       const unsigned startX, const unsigned startY) {
  array<unsigned char, 9> convertedGrid;

  unsigned numUnfilled = 0;
  unsigned i = 0;
  for (unsigned y = startY; y < startY + 3; y++) {
    for (unsigned x = startX; x < startX + 3; x++) {
      auto gv = grid[x + y * 9];

      if (gv == CellState::EMPTY) {
        numUnfilled++;
      }

      convertedGrid[i] = static_cast<unsigned char>(gv);
      i++;
    }
  }

  if (numUnfilled >= 7) {
    return TopCellState::UNDECIDED;
  }

  unsigned char run = findRun(convertedGrid);
  if (run == 0) {
    return numUnfilled == 0 ? TopCellState::DRAW : TopCellState::UNDECIDED;
  } else {
    return run == 1 ? TopCellState::NAUGHT : TopCellState::CROSS;
  }
}

static TopCellState calculateGridState(const array<TopCellState, NUM_TOP_CELLS> &grid) {
  array<unsigned char, 9> convertedGrid;

  unsigned numUnfilled = 0;
  unsigned numNeutral = 0;

  for (unsigned i = 0; i < NUM_TOP_CELLS; i++) {
    auto gv = grid[i];

    if (gv == TopCellState::UNDECIDED) {
      numNeutral++;
      numUnfilled++;
    } else if (gv == TopCellState::DRAW) {
      numNeutral++;
    }

    switch (gv) {
    case TopCellState::UNDECIDED:
    case TopCellState::DRAW:
      convertedGrid[i] = 0;
      break;
    case TopCellState::NAUGHT:
    case TopCellState::CROSS:
      convertedGrid[i] = static_cast<unsigned char>(gv);
      break;
    }
  }

  if (numNeutral >= 7) {
    return numUnfilled == 0 ? TopCellState::DRAW : TopCellState::UNDECIDED;
  }

  unsigned char run = findRun(convertedGrid);
  if (run == 0) {
    return numUnfilled == 0 ? TopCellState::DRAW : TopCellState::UNDECIDED;
  } else {
    return run == 1 ? TopCellState::NAUGHT : TopCellState::CROSS;
  }
}

State::State(unsigned char mySymbol) : mySymbol(mySymbol), isTerminal(false), isWin(false), isLoss(false) {
  cells.fill(CellState::EMPTY);
  topCells.fill(TopCellState::UNDECIDED);
}

State::State(const array<CellState, NUM_CELLS> &fieldCells, unsigned char mySymbol)
    : cells(fieldCells), mySymbol(mySymbol), isTerminal(false), isWin(false), isLoss(false) {

  unsigned i = 0;
  for (unsigned ty = 0; ty < 3; ty++) {
    for (unsigned tx = 0; tx < 3; tx++) {
      topCells[i++] = calculateGridState(cells, tx * 3, ty * 3);
    }
  }

  updateFlags();
}

State::State(const State &other)
    : cells(other.cells), topCells(other.topCells), mySymbol(other.mySymbol),
      isTerminal(other.isTerminal), isWin(other.isWin), isLoss(other.isLoss) {}

State& State::operator=(const State& other) {
    cells = other.cells;
    topCells = other.topCells;
    mySymbol = other.mySymbol;
    isTerminal = other.isTerminal;
    isWin = other.isWin;
    isLoss = other.isLoss;
    return *this;
}

bool State::operator==(const State &other) const {
  assert(cells.size() == other.cells.size());

  if (mySymbol != other.mySymbol) {
    return false;
  }

  for (unsigned i = 0; i < cells.size(); i++) {
    if (cells[i] != other.cells[i]) {
      return false;
    }
  }

  return true;
}

size_t State::HashCode() const {
    cout << "hash" << endl;
  size_t result = mySymbol;
  for (const auto &c : cells) {
    result = 3 * result + (c == CellState::EMPTY ? 0 : (c == CellState::NAUGHT ? 1 : 2));
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

  out << endl << "raw cells:" << endl;
  for (unsigned y = 0; y < 9; y++) {
    if (y%3 == 0) { out << endl; }
    for (unsigned x = 0; x < 9; x++) {
      if (x%3 == 0) { out << " "; }
      out << cells[x + y * 9];
    }
    out << endl;
  }
}

vector<Action> State::AvailableActions(void) const {
  vector<Action> result;
  if (IsTerminal()) {
    return result;
  }

  for (unsigned ty = 0; ty < 3; ty++) {
    for (unsigned tx = 0; tx < 3; tx++) {
      if (topCells[tx + ty * 3] == TopCellState::UNDECIDED) {
        unsigned sy = ty * 3;
        unsigned ey = sy + 3;
        unsigned sx = tx * 3;
        unsigned ex = sx + 3;

        for (unsigned y = sy; y < ey; y++) {
          for (unsigned x = sx; x < ex; x++) {
            unsigned char p = x + y * 9;
            if (cells[p] == CellState::EMPTY) {
              result.emplace_back(p);
            }
          }
        }
      }
    }
  }

  return result;
}

State State::SuccessorState(Action action) const {
  State result(*this);

  if (result.isTerminal || result.isWin || result.isLoss) {
     Output(cout);
  }
  assert(!result.isTerminal && !result.isWin && !result.isLoss);

  unsigned topX = (action % 9) / 3;
  unsigned topY = (action / 9) / 3;
  assert(result.topCells[topX + topY * 3] == TopCellState::UNDECIDED);

  assert(result.cells[action] == CellState::EMPTY);
  result.cells[action] = mySymbol == 1 ? CellState::NAUGHT : CellState::CROSS;

  auto newTopState = calculateGridState(result.cells, topX * 3, topY * 3);
  result.topCells[topX + topY * 3] = newTopState;

  if (newTopState != TopCellState::UNDECIDED) {
    result.updateFlags();
  }

  result.flipState();
  return result;
}

CellState State::GetCellState(unsigned cellIndex) const {
  return cells[cellIndex];
}

unsigned char State::GetMySymbol(void) const {
  return mySymbol;
}

bool State::IsTerminal(void) const { return isTerminal; }
bool State::IsWin(void) const { return isWin; }
bool State::IsLoss(void) const { return isLoss; }

void State::flipState(void) {
  mySymbol = mySymbol == 1 ? 2 : 1;
  swap(isWin, isLoss);
}

void State::updateFlags(void) {
  auto newGameState = calculateGridState(topCells);

  switch (newGameState) {
  case TopCellState::DRAW:
    isTerminal = true;
    isWin = false;
    isLoss = false;
    break;
  case TopCellState::UNDECIDED:
    isTerminal = false;
    isWin = false;
    isLoss = false;
    break;
  case TopCellState::NAUGHT:
  case TopCellState::CROSS:
    isTerminal = true;
    isWin = (mySymbol == static_cast<unsigned char>(newGameState));
    isLoss = !isWin;
    break;
  }
}
