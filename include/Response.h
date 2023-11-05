#pragma once

#include <vector>
#include <stdint.h>
#include "Packet.h"
#include "Request.h"
#include "SPoSLIP.h"

class Response : public Packet, public SPoSLIP {
public:
  virtual std::vector<uint8_t> serialize() const override = 0;
  virtual Request* deserialize(const std::vector<uint8_t>& data) = 0;

  uint8_t get_status() const { return status_; }
  void set_status(uint8_t status) { status_ = status; }

private:
  uint8_t status_;

};
