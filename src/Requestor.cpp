#include "Requestor.h"

Requestor::Requestor(Connection* connection)
  : connection_(connection) {}

std::unique_ptr<Response> Requestor::sendRequest(Request& request) {
  // Send the serialized request
  auto response_data = connection_->sendData(request.serialize());

  // Deserialize the response data into a Response object. Each Request type returns its twinned Response type.
  return request.deserialize(response_data);
}