#pragma once

#include <vector>
#include <stdint.h>
#include <stddef.h>
#include "Packet.h"

class Listener {
public:
  Listener() {};

  void sendPacket(const Packet& packet);
  std::vector<uint8_t> waitForResponse();

  void onStatusResponse(const std::vector<uint8_t>& response_data);
};