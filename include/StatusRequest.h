#pragma once

#include <vector>
#include <stdint.h>
#include "Request.h"
#include "Response.h"
#include "StatusResponse.h"

class StatusRequest : public Request {
public:
  uint8_t status_code;

  void serialize(std::vector<uint8_t>& data) const override;

  Response* deserialize(const std::vector<uint8_t>& data) override {
    return new StatusResponse(data);
  }
};