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
    std::unique_ptr<ReadBlockRequest> request = std::make_unique<ReadBlockRequest>(packet[0], packet[2]);
    request->set_block_number_from_ptr(packet.data(), 3);
    response = smart_port_handler_->read_block(static_cast<ReadBlockRequest*>(request.get()));
    break;
  }

  case SP_WRITE_BLOCK: {
    std::unique_ptr<WriteBlockRequest> request = std::make_unique<WriteBlockRequest>(packet[0], packet[2]);
    request->set_block_number_from_ptr(packet.data(), 3);
    request->set_block_data_from_ptr(packet.data(), 6);
    response = smart_port_handler_->write_block(static_cast<WriteBlockRequest*>(request.get()));
    break;
  }

  case SP_FORMAT: {
    std::unique_ptr<FormatRequest> request = std::make_unique<FormatRequest>(packet[0], packet[2]);
    response = smart_port_handler_->format(static_cast<FormatRequest*>(request.get()));
    break;
  }

  case SP_INIT: {
    std::unique_ptr<InitRequest> request = std::make_unique<InitRequest>(packet[0], packet[2]);
    response = smart_port_handler_->init(static_cast<InitRequest*>(request.get()));
    break;
  }

  case SP_OPEN: {
    std::unique_ptr<OpenRequest> request = std::make_unique<OpenRequest>(packet[0], packet[2]);
    response = smart_port_handler_->open(static_cast<OpenRequest*>(request.get()));
    break;
  }

  case SP_CLOSE: {
    std::unique_ptr<CloseRequest> request = std::make_unique<CloseRequest>(packet[0], packet[2]);
    response = smart_port_handler_->close(static_cast<CloseRequest*>(request.get()));
    break;
  }

  case SP_READ: {
    std::unique_ptr<ReadRequest> request = std::make_unique<ReadRequest>(packet[0], packet[2]);
    request->set_byte_count_from_ptr(packet.data(), 3);
    request->set_address_from_ptr(packet.data(), 5);
    response = smart_port_handler_->read(static_cast<ReadRequest*>(request.get()));
    break;
  }

  case SP_WRITE: {
    std::unique_ptr<WriteRequest> request = std::make_unique<WriteRequest>(packet[0], packet[2]);
    request->set_byte_count_from_ptr(packet.data(), 3);
    request->set_address_from_ptr(packet.data(), 5);
    request->set_data_from_ptr(packet.data(), 8, packet.size() - 8);
    response = smart_port_handler_->write(static_cast<WriteRequest*>(request.get()));
    break;
  }

  case SP_RESET: {
    std::unique_ptr<ResetRequest> request = std::make_unique<ResetRequest>(packet[0], packet[2]);
    response = smart_port_handler_->reset(static_cast<ResetRequest*>(request.get()));
    break;
  }

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