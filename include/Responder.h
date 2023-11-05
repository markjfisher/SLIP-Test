#pragma once

#include <vector>
#include <memory>
#include <stdint.h>
#include <iostream>

#include "SmartPortHandler.h"

class Responder {

public:
  Responder(std::unique_ptr<SmartPortHandler> handler)
    : smartPortHandler_(std::move(handler)) {
      std::cout << "Creating Responder" << std::endl;
    }

  std::vector<uint8_t> process(const std::vector<uint8_t>& packet);

private:
  std::unique_ptr<SmartPortHandler> smartPortHandler_;
};