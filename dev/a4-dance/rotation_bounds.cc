#include "rotation_bounds.h"
#include <cstdlib>

RotationBounds::RotationBounds() {
  dofRX = false;
  dofRY = false;
  dofRZ = false;
  minRX = 0;
  maxRX = 0;
  minRY = 0;
  maxRY = 0;
  minRZ = 0;
  maxRZ = 0;
  dofs = 0;
}

void RotationBounds::setdof(bool rx, bool ry, bool rz) {
  dofRX = rx;
  dofRY = ry;
  dofRZ = rz;
  dofs = rx + ry + rz;
}

void RotationBounds::setR(int index, float min, float max) {
  if (index > dofs) {
    std::abort(); // Trying to set past the max index.
  }
  if (index == 0) {
    if (dofRX) {
      minRX = min;
      maxRX = max;
    } else if (dofRY) {
      minRY = min;
      maxRY = max;
    } else if (dofRZ) {
      minRZ = min;
      maxRZ = max;
    } else {
      abort();
    }
  } else if (index == 1) {
    if (dofRX && dofRY) {
      minRY = min;
      maxRY = max;
    } else if (dofRX && dofRZ) {
      minRZ = min;
      maxRZ = max;
    } else if (dofRY && dofRZ) {
      minRZ = min;
      maxRZ = max;
    } else {
      abort();
    }
  } else if (index == 2) {
    if (dofRX && dofRY && dofRZ) {
      minRZ = min;
      maxRZ = max;
    } else {
      abort();
    }
  } else {
    abort();
  }
}