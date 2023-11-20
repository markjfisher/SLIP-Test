#include <stdexcept>
#include <memory>
#include <sstream>
#include <memory>
#include "Responder.h"

#include "CloseRequest.h"
#include "CloseResponse.h"
#include "ControlRequest.h"
#include "ControlResponse.h"
#include "FormatRequest.h"
#include "FormatResponse.h"
#include "InitRequest.h"
#include "InitResponse.h"
#include "OpenRequest.h"
#include "OpenResponse.h"
#include "ReadBlockRequest.h"
#include "ReadBlockResponse.h"
#include "ReadRequest.h"
#include "ReadResponse.h"
#include "ResetRequest.h"
#include "ResetResponse.h"
#include "StatusRequest.h"
#include "StatusResponse.h"
#include "WriteBlockRequest.h"
#include "WriteBlockResponse.h"
#include "WriteRequest.h"
#include "WriteResponse.h"

void Responder::processRequestData(const std::vector<uint8_t>& packet) {
  // Convert each packet's "command" into the appropriate Request object, let the smart port handler process it

  std::unique_ptr<Response> response;

  uint8_t command = packet[1];

#ifdef DEBUG
  std::cout << "Responder::processRequestData, command: " << static_cast<unsigned int>(command) << std::endl;
#endif

  switch(command) {

  case SP_STATUS: {
    std::unique_ptr<StatusRequest> request = std::make_unique<StatusRequest>(packet[0], packet[2], packet[3]);
    response = smart_port_handler_->status(static_cast<StatusRequest*>(request.get()));
    break;
  }

  case SP_CONTROL: {
    std::vector<uint8_t> payload(packet.begin() + 3, packet.end());
    std::unique_ptr<ControlRequest> request = std::make_unique<ControlRequest>(packet[0], packet[2], packet[3], payload);
    response = smart_port_handler_->control(static_cast<ControlRequest*>(request.get()));
    break;
  }

  case SP_READ_BLOCK: {
    // std::unique_ptr<ReadBlockRequest> request = std::make_unique<ReadBlockRequest>(packet[0], packet[2], packet[3]);
    break;
  }
  case SP_WRITE_BLOCK:
  case SP_FORMAT:
  case SP_INIT:
  case SP_OPEN:
  case SP_CLOSE:
  case SP_READ:
  case SP_WRITE:
  case SP_RESET:
    throw std::runtime_error("Not yet implemented");
    break;

  default: {
    std::ostringstream oss;
    oss << "Unknown command: %d" << command;
    throw std::runtime_error(oss.str());
    break;
  }

  }

  if (response == nullptr) {
    std::cout << "Response was null. Not returning anything" << std::endl;
    return;
  }

  auto response_data = response->serialize();
  connection_->send_data(response_data);
}

void Responder::wait_for_requests() {
  while (true) {
    // Keep listening for incoming requests
    auto request_data = connection_->wait_for_request();
    processRequestData(request_data);
  }
}