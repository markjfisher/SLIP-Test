#pragma once

#include <vector>
#include <stdint.h>
#include "Listener.h"

class Requestor {
public:
  Requestor(Listener& listener) : listener_(listener) {}

  std::vector<uint8_t> getStatus();

private:
  Listener& listener_;
};