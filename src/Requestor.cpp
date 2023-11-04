
#include <vector>
#include <stdint.h>
#include "SLIP.h"
#include "Requestor.h"
#include "StatusRequest.h"
#include "StatusResponse.h"

std::vector<uint8_t> Requestor::getStatus() {
    // Create a StatusRequest packet
    StatusRequest request;

    // Send the request to the Responder
    listener.sendPacket(request);

    // Wait for the response
    std::vector<uint8_t> response = listener.waitForResponse();

    // Split the response into SLIP packets
    std::vector<std::vector<uint8_t>> slip_packets = SLIP::splitIntoSlipPackets(response.data(), response.size());

    // Extract the StatusResponse data from the SLIP packets
    StatusResponse status_response;
    for (const auto& slip_packet : slip_packets) {
        status_response.deserialize(slip_packet.data(), slip_packet.size());
    }

    return status_response.data;
}

