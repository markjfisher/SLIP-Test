#include <iostream>
#include <stddef.h>
#include "SLIP.h"
#include "Util.h"

std::vector<uint8_t> SLIP::encode(const std::vector<uint8_t>& data) {
  std::vector<uint8_t> encoded_data;

  // start with SLIP_END
  encoded_data.push_back(SLIP_END);

  // Escape any SLIP special characters in the packet data
  for (uint8_t byte : data) {
    if (byte == SLIP_END || byte == SLIP_ESC) {
      encoded_data.push_back(SLIP_ESC);
      encoded_data.push_back(byte == SLIP_END ? SLIP_ESC_END : SLIP_ESC_ESC);
    } else {
      encoded_data.push_back(byte);
    }
  }

  // Add the SLIP END byte to the end of the encoded data
  encoded_data.push_back(SLIP_END);

  return encoded_data;
}

std::vector<uint8_t> SLIP::decode(const std::vector<uint8_t>& data) {
  std::cout << "SLIP::decode" << std::endl;
  Util::hex_dump(data);


  std::vector<uint8_t> decoded_data;
  auto bytes_read = data.size();

  size_t i = 0;
  while (i < bytes_read) {
    if (data[i] == SLIP_END) {
      // Start of a SLIP packet
      i++;

      // Decode the SLIP packet data
      while (i < bytes_read && data[i] != SLIP_END) {
        if (data[i] == SLIP_ESC) {
          // Escaped byte
          i++;

          if (data[i] == SLIP_ESC_END) {
            // Escaped END byte
            decoded_data.push_back(SLIP_END);
          } else if (data[i] == SLIP_ESC_ESC) {
            // Escaped ESC byte
            decoded_data.push_back(SLIP_ESC);
          } else {
            // Invalid escape sequence
            return std::vector<uint8_t>();
          }
        } else {
          // Non-escaped byte
          decoded_data.push_back(data[i]);
        }

        i++;
      }

      // Check for end of packet
      if (i == bytes_read || data[i] != SLIP_END) {
        // Incomplete SLIP packet
        return std::vector<uint8_t>();
      }
    } else {
      // Invalid SLIP packet
      return std::vector<uint8_t>();
    }

    i++;
  }

  std::cout << "SLIP::decode, sending decoded data:" << std::endl;
  Util::hex_dump(decoded_data);

  return decoded_data;
}


std::vector<std::vector<uint8_t>> SLIP::splitIntoPackets(const uint8_t* data, size_t bytes_read) {
  // The list of decoded SLIP packets
  std::vector<std::vector<uint8_t>> packets;

  enum class State {
    NotParsing,
    Parsing
  } state = State::NotParsing;

  // Iterate over the data and find the SLIP packet boundaries
  size_t i = 0;
  while (i < bytes_read) {
    switch (state) {
      case State::NotParsing:
        // If we see a SLIP_END byte, start parsing a new SLIP packet
        if (data[i] == SLIP_END) {
          state = State::Parsing;
        }
        break;
      case State::Parsing:
        // If we see another SLIP_END byte, we have reached the end of the SLIP packet
        if (data[i] == SLIP_END) {
          // Extract the SLIP packet data
          std::vector<uint8_t> slip_packet_data(data + i - 1, data + i);

          // Add the SLIP packet data to the list of SLIP packets
          packets.push_back(SLIP::decode(slip_packet_data));

          // Transition back to the NotParsing state
          state = State::NotParsing;
        }
        break;
    }
    i++;
  }

  return packets;
}