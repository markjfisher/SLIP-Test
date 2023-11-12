#include <vector>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include "Connection.h"
#include "Util.h"

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

std::vector<uint8_t> Connection::waitForResponse(int requestId, std::chrono::seconds timeout) {
  std::unique_lock<std::mutex> lock(responses_mutex_);
  // mutex is unlocked as it goes into a wait, so then the inserting thread can add to map, and this can then pick it up when notified, or timeout.
  if(!response_cv_.wait_for(lock, timeout, [this, requestId]() { return responses_.count(requestId) > 0; })) {
    throw std::runtime_error("Timeout waiting for response");
  }
  std::vector<uint8_t> responseData = responses_[requestId];
  responses_.erase(requestId);
  return responseData;
}

std::vector<uint8_t> Connection::waitForRequest() {
  std::unique_lock<std::mutex> lock(responses_mutex_);
  response_cv_.wait(lock, [this]() { return responses_.size() > 0; });
  auto it = responses_.begin();
  std::vector<uint8_t> requestData = it->second;
  responses_.erase(it);

#ifdef DEBUG
  std::cout << "Connection::waitForRequest, request data:" << std::endl;
  Util::hex_dump(requestData);
#endif

  return requestData;
}

std::string Connection::toString() {
  std::stringstream ss;
  ss << "Connection: isConnected = " << (is_connected_ ? "true" : "false") << ", devices = [";
  for (auto it = devices_.begin(); it != devices_.end(); ++it) {
    ss << "{deviceIndex = " << it->deviceIndex
       << ", capability = {unitId = " << it->capability.unitId
       << ", name = " << it->capability.name << "}}";
    if (std::next(it) != devices_.end()) {
      ss << ", ";
    }
  }
  ss << "]";
  return ss.str();
}