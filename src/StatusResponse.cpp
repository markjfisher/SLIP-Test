#include <stddef.h>
#include "StatusResponse.h"
#include "StatusRequest.h"
#include <stdexcept>

std::vector<uint8_t> StatusResponse::serialize() const {
  std::vector<uint8_t> data;
  data.push_back(this->get_request_sequence_number());
  data.push_back(this->get_status());

  for (uint8_t status_value : get_status_values()) {
    data.push_back(status_value);
  }
  return data;
}

Request* StatusResponse::deserialize(const std::vector<uint8_t>& data)  {
  if (data.size() < 4) {
    throw std::runtime_error("Not enough data to deserialize StatusResponse");
  }

  StatusRequest* request = new StatusRequest();
  request->set_request_sequence_number(data[0]);
  request->set_command_number(data[1]);
  request->set_sp_unit(data[2]);
  request->set_status_code(data[3]);

  return request;
}
