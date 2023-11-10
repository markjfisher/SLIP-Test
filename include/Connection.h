#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include <string>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <chrono>

class Connection {
public:
  struct SmartPortCapability {
    int unitId;
    std::string name;
  };

  struct Device {
    int deviceIndex;
    SmartPortCapability capability;
  };

  virtual ~Connection() = default;
  virtual void sendData(const std::vector<uint8_t>& data) = 0;

  virtual void createReadChannel() = 0;

  void addDevices(const std::vector<uint8_t>& data);
  const std::vector<Device>& getDevices() const;
  const Device* findDevice(int deviceId) const;

  bool isConnected() { return is_connected_; }
  void setIsConnected(bool is_connected) { is_connected_ = is_connected; }

  std::vector<uint8_t> waitForResponse(int requestId, std::chrono::seconds timeout);

private:
  bool is_connected_;
  std::vector<Device> devices_;
  static int deviceIndex_;

protected:
  std::map<uint8_t, std::vector<uint8_t>> responses_;

  std::mutex responses_mutex_;
  std::condition_variable response_cv_;
};
