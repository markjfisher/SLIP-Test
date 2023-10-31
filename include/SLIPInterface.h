#ifndef SLIP_INTERFACE_H
#define SLIP_INTERFACE_H

#include <exception>
#include <memory>
#include <vector>
#include <stdint.h>

#include "BaseDevice.h"

#define SLIP_START_BYTE 0x7E
#define SLIP_END_BYTE 0x7D
#define SLIP_ESCAPE_BYTE 0x5E
#define SLIP_ESCAPE_MASK 0x20

class SLIPInterface {
public:
  SLIPInterface(std::unique_ptr<BaseDevice> device);

  // Reads a SLIP packet from the interface.
  std::vector<uint8_t> read();

  // Writes a SLIP packet to the interface.
  void write(const std::vector<uint8_t>& data);

  // Checks if the interface has any data.
  bool has_data();

private:
  std::unique_ptr<BaseDevice> device_;
  std::vector<uint8_t> receive_buffer_;

  static std::vector<uint8_t> encode(const std::vector<uint8_t>& data);
  static std::vector<uint8_t> decode(const std::vector<uint8_t>& data);

};

#endif
