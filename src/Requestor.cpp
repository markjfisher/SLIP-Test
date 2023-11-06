#include <iostream>
#include "Requestor.h"
#include "Listener.h"

std::unique_ptr<Response> Requestor::sendRequest(const Request& request) {
  std::cout << "Requestor::sendRequest" << std::endl;

  // get a connection that will service our request
  auto connection = listener_->findConnectionWithDevice(request.get_sp_unit());

  // Send the serialized request
  auto response_data = connection->sendData(request.serialize());

  // Deserialize the response data into a Response object.
  // Each Request type (e.g. StatusRequest) is able to deserialize into its twin Response (e.g. StatusResponse).
  return request.deserialize(response_data);
}