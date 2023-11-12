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
    : smart_port_handler_(std::move(handler)), connection_(connection) {}

  void processRequestData(const std::vector<uint8_t>& packet);
  void wait_for_requests();

private:
  std::unique_ptr<SmartPortHandler> smart_port_handler_;
  std::shared_ptr<Connection> connection_;
};