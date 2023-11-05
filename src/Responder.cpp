#include <stdexcept>
#include <memory>
#include <sstream>
#include <memory>
#include "SLIP.h"
#include "Responder.h"

std::vector<uint8_t> Responder::process(const std::vector<uint8_t>& packet) {
  // Convert each packet's "command" into the appropriate Request object, let the smart port handler process it, then slip encode the response and return the data

  std::unique_ptr<Response> response;
  std::unique_ptr<Request> request;

  uint8_t command = packet[1];
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
  }
  }

  return SLIP::encode(response->serialize());
}
