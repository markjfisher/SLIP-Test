#ifndef BASE_DEVICE_H
#define BASE_DEVICE_H

#include <stdint.h>
#include <vector>

class BaseDevice {
public:
  virtual void Write(const std::vector<uint8_t>& data) = 0;
  virtual std::vector<uint8_t> Read() = 0;
};

#endif