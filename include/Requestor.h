#pragma once

#include <vector>
#include <stdint.h>
#include "Listener.h"

class Requestor {
public:
  Requestor(Listener& listener) : listener_(listener) {}

private:
  Listener& listener_;
};