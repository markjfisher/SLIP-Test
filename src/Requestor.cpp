
#include <vector>
#include <stdint.h>
#include <iostream>

// #include "SLIP.h"
#include "Requestor.h"
#include "StatusRequest.h"
#include "StatusResponse.h"

Requestor::Requestor(Connection* connection, Listener* listener)
  : connection_(connection), listener_(listener) {}

Response* Requestor::sendRequest(Request& request) {
  // Add the request to the map of sent requests
  sent_requests_[request.get_request_sequence_number()] = request;

  // Send the serialized request
  connection_->sendData(request.serialize());

  // Wait for the response
  std::unique_lock<std::mutex> lock(mtx_);
  if(!cv_.wait_for(lock, std::chrono::seconds(REQUESTOR_TIMEOUT), [this, &request]() {
    return listener_->hasResponse(request.get_request_sequence_number());
  })) {
    // Timeout
    std::cerr << "Timeout waiting for response" << std::endl;
    return nullptr;
  }

  // Get the response data
  auto response_data = listener_->getPacket(request.get_request_sequence_number());

  // Deserialize the response data into a Response object. Each Request type returns the appropriate Response type.
  Response* response = request.deserialize(response_data);

  return response;
}