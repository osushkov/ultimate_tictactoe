#pragma once

namespace fastbot {

struct Spec {
  float explorationWeightA;
  float explorationWeightC;
  float randomWeight;
  float pRandomSelect;

  Spec(float explorationWeightA, float explorationWeightC, float randomWeight, float pRandomSelect)
      : explorationWeightA(explorationWeightA), explorationWeightC(explorationWeightC),
        randomWeight(randomWeight), pRandomSelect(pRandomSelect) {}
};
}
