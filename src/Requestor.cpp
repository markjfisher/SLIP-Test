#include <iostream>
#include "Requestor.h"

Requestor::Requestor(std::unique_ptr<Connection> connection)
  : connection_(std::move(connection)) {}

std::unique_ptr<Response> Requestor::sendRequest(const Request& request) {
  std::cout << "Requestor::sendRequest" << std::endl;

  // Send the serialized request
  auto response_data = connection_->sendData(request.serialize());

  // Deserialize the response data into a Response object. Each Request type returns its twinned Response type.
  return request.deserialize(response_data);
}