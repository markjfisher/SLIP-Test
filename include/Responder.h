#pragma once

#include <vector>
#include <memory>
#include <stdint.h>

#include "SmartPortHandler.h"

class Responder {

public:
  Responder(std::unique_ptr<SmartPortHandler> handler)
    : smartPortHandler_(std::move(handler)) {}

  std::vector<uint8_t> process(const std::vector<uint8_t>& packet);

private:
  std::unique_ptr<SmartPortHandler> smartPortHandler_;
};