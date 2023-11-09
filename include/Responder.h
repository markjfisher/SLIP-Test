#pragma once

#include <vector>
#include <memory>
#include <stdint.h>
#include <iostream>

#include "SmartPortHandler.h"

// A Responder class is independent of any Connection implementation.
// It uses its SmartPortHandler to process requests, and converts that into the serialized vector of bytes to be returned.
// It is the Connection implementation (which owns the Responder) that decides how to send the response data back.
// No SLIP encoding or decoding done here, everything is based on serialized objects.
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