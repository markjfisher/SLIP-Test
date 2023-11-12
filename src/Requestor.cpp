#include <iostream>
#include "Requestor.h"
#include "Listener.h"

std::unique_ptr<Response> Requestor::sendRequest(const Request& request) {

#ifdef DEBUG
  std::cout << "Requestor::sendRequest, request: " << request << std::endl;
#endif

  // get a connection that will service our request
  auto connection = listener_->findConnectionWithDevice(request.get_sp_unit());

  if (connection == nullptr) {
    std::cout << "No device found that could service unit " << static_cast<unsigned int>(request.get_sp_unit()) << std::endl;
    return nullptr;
  }

  // Send the serialized request
  connection->sendData(request.serialize());

  std::vector<uint8_t> response_data;
  try {
    response_data = connection->waitForResponse(request.get_request_sequence_number(), std::chrono::seconds(20));
  } catch (const std::runtime_error& e) {
    std::cerr << "Requestor::sendRequest did not get response, error = " << e.what() << std::endl;
    return nullptr;
  }

  // Deserialize the response data into a Response object.
  // Each Request type (e.g. StatusRequest) is able to deserialize into its twin Response (e.g. StatusResponse).
  return request.deserialize(response_data);
}