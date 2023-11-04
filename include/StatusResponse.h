#pragma once

#include <vector>
#include <stdint.h>
#include "Response.h"

class StatusResponse : public Response {
public:
  void serialize(std::vector<uint8_t>& data) const;
  // void deserialize(const std::vector<uint8_t>& data) override;

// public:
//   void from(const std::vector<uint8_t>& data);
//   void serialize(std::vector<uint8_t>& data) const override {}
};