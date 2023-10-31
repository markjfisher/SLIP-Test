#include <iostream>
#include <string>
#include <vector>
#include "SLIPInterface.h"

SLIPInterface::SLIPInterface(std::unique_ptr<BaseDevice> device) : device_(std::move(device)) {}

std::vector<uint8_t> SLIPInterface::encode(const std::vector<uint8_t>& data) {
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

std::vector<uint8_t> SLIPInterface::decode(const std::vector<uint8_t>& data) {
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

std::vector<uint8_t> SLIPInterface::read() {
  if (receive_buffer_.empty()) {
    // Read data from the physical interface.
    std::vector<uint8_t> data;
    data = device_->Read();

    // Append the data to the receive buffer.
    receive_buffer_.insert(receive_buffer_.end(), data.begin(), data.end());
  }

  std::vector<uint8_t> packet = decode(receive_buffer_);

  // Remove the decoded packet from the receive buffer.
  receive_buffer_.erase(receive_buffer_.begin(), receive_buffer_.begin() + packet.size());

  // Return the decoded packet.
  return packet;
}

void SLIPInterface::write(const std::vector<uint8_t>& data) {
  device_->Write(encode(data));
}

bool SLIPInterface::has_data() {
  return !receive_buffer_.empty();
}