#ifndef SLIP_INTERFACE_H
#define SLIP_INTERFACE_H

#include <exception>
#include "BaseDevice/BaseDevice.h"

class SLIPInterface {
public:
  SLIPInterface(std::unique_ptr<BaseDevice> device) : device_(std::move(device)) {}

std::vector<uint8_t> encode(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> packet;

    // Add an END marker to the beginning of the packet.
    packet.push_back(0x7E);

    // Escape any ESC or END characters in the data.
    for (uint8_t byte : data) {
      if (byte == 0x7D || byte == 0x7E) {
        packet.push_back(0x7D);
        packet.push_back(byte ^ 0x20);
      } else {
        packet.push_back(byte);
      }
    }

    // Add an END marker to the end of the packet.
    packet.push_back(0x7E);

    return packet;
  }

  std::vector<uint8_t> decode(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> decoded_data;

    // Check for the END marker at the beginning of the packet.
    if (data[0] != 0x7E) {
      throw std::runtime_error("Invalid SLIP packet");
    }

    // Iterate over the data, escaping any escaped characters.
    for (size_t i = 1; i < data.size() - 1; i++) {
      if (data[i] == 0x7D) {
        decoded_data.push_back(data[i + 1] ^ 0x20);
        i++;
      } else {
        decoded_data.push_back(data[i]);
      }
    }

    // Check for the END marker at the end of the packet.
    if (data[data.size() - 1] != 0x7E) {
      throw std::runtime_error("Invalid SLIP packet");
    }

    return decoded_data;
  }

  std::vector<uint8_t> read() {
    // Read data from the device.
    return device_->Read();
  }

  void write(const std::vector<uint8_t>& data) {
    // Write data to the device.
    device_->Write(data);
  }

private:
  std::unique_ptr<BaseDevice> device_;
};


#endif
