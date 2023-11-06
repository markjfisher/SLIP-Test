#include <vector>
#include <cstdint>
#include <iostream>
#include "Connection.h"

int Connection::deviceIndex_ = 0;

const std::vector<Connection::Device>& Connection::getDevices() const {
  return devices_;
}

const Connection::Device* Connection::findDevice(int deviceId) const {
  for (const auto& device : devices_) {
    if (device.deviceIndex == deviceId) {
      return &device;
    }
  }
  return nullptr;
}

// Convert
void Connection::addDevices(const std::vector<uint8_t>& data) {
  size_t i = 0;
  while (i < data.size()) {
    // There should be at least one more byte for the unit number
    if (i >= data.size()) {
      // Handle the error...
      break;
    }

    // Take one byte as the unit number
    int unitId = data[i++];
    
    // Take the remaining bytes as the unit name until \0 is found
    std::string unitName;
    while (i < data.size() && data[i] != 0) {
      unitName.push_back(static_cast<char>(data[i++]));
    }

    // Skip the null character
    if (i < data.size()) {
      ++i;
    }

    // Add the Device to the devices vector
    devices_.push_back({++deviceIndex_, {unitId, unitName}});
  }
}