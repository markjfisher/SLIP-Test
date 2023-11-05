#include "StatusRequest.h"
#include "StatusResponse.h"
#include <stdexcept>

std::vector<uint8_t> StatusRequest::serialize() const {
  std::vector<uint8_t> data;
  data.push_back(this->get_request_sequence_number());
  data.push_back(0);
  data.push_back(this->get_sp_unit());
  data.push_back(this->get_status_code());

  return data;
}

Response* StatusRequest::deserialize(const std::vector<uint8_t>& data) {
  if (data.size() < 2) {
    throw std::runtime_error("Not enough data to deserialize StatusRequest");
  }

  StatusResponse* response = new StatusResponse();
  response->set_request_sequence_number(data[0]);
  response->set_status(data[1]);

  if (response->get_status() == 0 && data.size() > 2) {
    for (size_t i = 2; i < data.size(); ++i) {
      response->add_status_value(data[i]);
    }
  }

  return response;
}