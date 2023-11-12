#include "FakeSmartPortHandler.h"
#include "StatusResponse.h"

std::unique_ptr<Response> FakeSmartPortHandler::status(StatusRequest* request) {
  std::cout << "FakeSmartPortHandler::status called with request: " << request->toString() << std::endl;
  auto response = std::make_unique<StatusResponse>();
  response->set_request_sequence_number(request->get_request_sequence_number());
  response->set_status(1);
  return response;
}