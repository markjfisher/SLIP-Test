#ifndef SLIP_INTERFACE_H
#define SLIP_INTERFACE_H

#include <exception>
#include "BaseDevice.h"

#include <vector>
#include <stdint.h>

#define SLIP_START_BYTE 0x7E
#define SLIP_END_BYTE 0x7D
#define SLIP_ESCAPE_BYTE 0x5E
#define SLIP_ESCAPE_MASK 0x20

class SLIPInterface {
public:
  SLIPInterface(std::unique_ptr<BaseDevice> device) : device_(std::move(device)) {}

  // Encodes a vector of bytes into a SLIP frame.
  // Returns a vector of bytes containing the SLIP frame.
  static std::vector<uint8_t> encode(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> slip_frame;

    // Add the start of frame byte.
    slip_frame.push_back(SLIP_START_BYTE);

    // Encode the data bytes.
    for (uint8_t byte : data) {
      if (byte == SLIP_START_BYTE || byte == SLIP_END_BYTE || byte == SLIP_ESCAPE_BYTE) {
        // Escape the byte.
        slip_frame.push_back(SLIP_ESCAPE_BYTE);
        slip_frame.push_back(byte ^ SLIP_ESCAPE_MASK);
      } else {
        // Add the byte to the SLIP frame.
        slip_frame.push_back(byte);
      }
    }

    // Add the end of frame byte.
    slip_frame.push_back(SLIP_END_BYTE);

    return slip_frame;
  }

  // Decodes a SLIP frame into a vector of bytes.
  // Returns a vector of bytes containing the decoded data.
  static std::vector<uint8_t> decode(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> decoded_data;

    // Check for the start of frame byte.
    if (data[0] != SLIP_START_BYTE) {
      // Invalid SLIP frame.
      return decoded_data;
    }

    // Decode the data bytes.
    for (int i = 1; i < data.size() - 1; i++) {
      if (data[i] == SLIP_ESCAPE_BYTE) {
        // Escape byte.
        decoded_data.push_back(data[i + 1] ^ SLIP_ESCAPE_MASK);
        i++;
      } else {
        // Regular byte.
        decoded_data.push_back(data[i]);
      }
    }

    // Check for the end of frame byte.
    if (data[data.size() - 1] != SLIP_END_BYTE) {
      // Invalid SLIP frame.
      return decoded_data;
    }

    return decoded_data;
  }

  std::vector<uint8_t> read() {
    return device_->Read();
  }

  void write(const std::vector<uint8_t>& data) {
    device_->Write(encode(data));
  }

private:
  std::unique_ptr<BaseDevice> device_;
};


#endif
