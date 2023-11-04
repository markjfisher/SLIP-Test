#include <vector>
#include <stdint.h>
#include "Listener.h"
#include "Packet.h"
#include "SLIP.h"

void Listener::sendPacket(const Packet& packet) {
  std::vector<uint8_t> encoded_data = SLIP::encode(packet);
  // SEND IT!
}

std::vector<uint8_t> Listener::waitForResponse() {
    // Receive data over the network
    std::vector<uint8_t> received_data;

    // GET IT!

    // Decode the received data
    std::vector<uint8_t> decoded_data = SLIP::decode(received_data.data(), received_data.size());

    return decoded_data;
}

void Listener::onStatusResponse(const std::vector<uint8_t>& response_data) {
}