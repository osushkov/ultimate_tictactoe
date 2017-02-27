#pragma once

#include <cmath>

namespace fastbot {

struct Spec {
  float explorationWeightA;
  float explorationWeightC;
  float randomWeight;
  float pRandomSelect;

  Spec() = default;
  Spec(float explorationWeightA, float explorationWeightC, float randomWeight, float pRandomSelect)
      : explorationWeightA(explorationWeightA), explorationWeightC(explorationWeightC),
        randomWeight(randomWeight), pRandomSelect(pRandomSelect) {}

  bool operator==(const Spec &other) {
     return fabsf(explorationWeightA - other.explorationWeightA) < 0.0001f &&
        fabsf(explorationWeightC - other.explorationWeightC) < 0.0001f &&
        fabsf(randomWeight - other.randomWeight) < 0.0001f &&
        fabsf(pRandomSelect - other.pRandomSelect) < 0.0001f;
  }

  bool operator!=(const Spec &other) {
    return !(*this == other);
  }
};
}
