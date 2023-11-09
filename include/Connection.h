#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <utility>

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
  virtual std::vector<uint8_t> sendData(const std::vector<uint8_t>& data) = 0;

  virtual void createReadChannel() = 0;

  void addDevices(const std::vector<uint8_t>& data);
  const std::vector<Device>& getDevices() const;
  const Device* findDevice(int deviceId) const;

  bool isConnected() { return is_connected_; }
  void setIsConnected(bool is_connected) { is_connected_ = is_connected; }

private:
  bool is_connected_;
  std::vector<Device> devices_;
  static int deviceIndex_;
};
