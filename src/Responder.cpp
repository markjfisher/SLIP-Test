#include <stdexcept>
#include <memory>
#include <sstream>
#include <memory>
#include "Responder.h"

void Responder::processRequestData(const std::vector<uint8_t>& packet) {
  // Convert each packet's "command" into the appropriate Request object, let the smart port handler process it

  std::unique_ptr<Response> response;
  std::unique_ptr<Request> request;

  uint8_t command = packet[1];
  std::cout << "Responder::processRequestData for command: " << command << std::endl;
  switch(command) {

  case SP_STATUS:
    request = std::make_unique<StatusRequest>(packet[0], packet[2], packet[3]);
    response = smartPortHandler_->status(static_cast<StatusRequest*>(request.get()));
    break;

  // case SP_READ_BLOCK:
  //   request = std::make_unique<ReadBlockRequest>(packet[0], packet[2], packet[3]);
  //   response = smartPortHandler_->readBlock(static_cast<ReadBlockRequest*>(request.get()));
  //   break;

  // TODO: ALL OTHER COMMANDS HERE
  case SP_READ_BLOCK:
  case SP_WRITE_BLOCK:
  case SP_FORMAT:
  case SP_CONTROL:
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

  auto response_data = response->serialize();
  connection_->sendData(response_data);
}

void Responder::waitForRequests() {
  std::cout << "starting waitForRequests in responder" << std::endl;
  while (true) {
    auto request_data = connection_->waitForRequest();
    processRequestData(request_data);
  }
}