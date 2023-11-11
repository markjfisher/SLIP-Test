#pragma once

#include <vector>
#include <memory>
#include <stdint.h>
#include <iostream>

#include "SmartPortHandler.h"
#include "Connection.h"

// A Responder class
// It uses its SmartPortHandler to process requests, and converts that into the serialized vector of bytes to be returned.
class Responder {

public:
  Responder(std::unique_ptr<SmartPortHandler> handler, std::shared_ptr<Connection> connection)
    : smartPortHandler_(std::move(handler)), connection_(connection) {}

  void processRequestData(const std::vector<uint8_t>& packet);
  void waitForRequests();

private:
  std::unique_ptr<SmartPortHandler> smartPortHandler_;
  std::shared_ptr<Connection> connection_;
};