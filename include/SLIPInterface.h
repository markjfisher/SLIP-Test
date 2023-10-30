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

  // Encodes a vector of bytes into a SLIP frame.
  // Returns a vector of bytes containing the SLIP frame.
  static std::vector<uint8_t> encode(const std::vector<uint8_t>& data);

  // Decodes a SLIP frame into a vector of bytes.
  // Returns a vector of bytes containing the decoded data.
  static std::vector<uint8_t> decode(const std::vector<uint8_t>& data);

  // Reads a SLIP packet from the interface.
  std::vector<uint8_t> read();

  // Writes a SLIP packet to the interface.
  void write(const std::vector<uint8_t>& data);

private:
  std::unique_ptr<BaseDevice> device_;
};

#endif
