#include <sstream>

#include "FakeSmartPortHandler.h"
#include "StatusResponse.h"
#include "Util.h"

void FakeSmartPortHandler::parse_capabilities(const std::string& input) {
  std::istringstream iss(input);
  std::string token;
  while (std::getline(iss, token, ',')) {
      int id = std::stoi(token);
      if (std::getline(iss, token, ',')) {
          std::cout << "adding " << token << " to map at id: " << id << std::endl;
          capability_map_[id] = token;
      }
  }
}

std::unique_ptr<StatusResponse> FakeSmartPortHandler::status(StatusRequest* request) {
  std::cout << "FakeSmartPortHandler::status called with request: " << std::endl;

  if (request->get_sp_unit() != 0x00 && request->get_status_code() == 0x03) {
    // DIB request, get the capability for this unit number
    auto it = capability_map_.find(request->get_sp_unit());
    if (it != capability_map_.end()) {
      auto name = it->second;
      std::vector<uint8_t> data;
      data.push_back(1 << 5 | 1 << 4); // READ_ALLOWED | DEVICE_ONLINE
      data.push_back(0); // block size 1
      data.push_back(0); // block size 2
      data.push_back(0); // block size 3
      data.push_back(name.size());
      data.insert(data.end(), name.begin(), name.end());
      int spaces_to_append = 16 - name.size();
      for (int i=0; i < spaces_to_append; i++) {
        data.push_back(static_cast<uint8_t>(' '));
      }
      // In this test endpoint, return a FUJINET BYTE device 0x10, with subtype 0x00
      data.push_back(0x10);
      data.push_back(0x00);
      // Followed by Firmware Version
      data.push_back(0x00);
      data.push_back(0x01);
      auto response = std::make_unique<StatusResponse>(request->get_request_sequence_number(), 0);
      response->set_data(data);
      std::cout << "created DIB response:" << std::endl;
      Util::hex_dump(response->serialize());
      return response;
    } else {
      // return an error response, the id wasn't here.
      // Shouldn't be possible as we are accumulating the capabilities and the server will know which connection
      // owns each device id.
      std::cerr << "FakeSmartPortHandler::status - DIB request for id " << static_cast<unsigned int>(request->get_sp_unit());
      std::cerr << " which should not be possible." << std::endl;
      return nullptr;
    }
  }

  if (request->get_sp_unit() == 0 && request->get_status_code() == 0) {
    // DEVICE Count
    auto response = std::make_unique<StatusResponse>(request->get_request_sequence_number(), 0);
    std::cout << "sending response length: " << capability_map_.size() << std::endl;
    response->add_data(capability_map_.size());
    return response;
  }


  // TODO: change the upper range to be relevant to number of devices currently attached.
  if (request->get_sp_unit() <= 2) {
    auto response = std::make_unique<StatusResponse>(request->get_request_sequence_number(), 0);
    response->add_data(1 << 6 | 1 << 5 | 1 << 4); // READ/WRITE ALLOWED, ONLINE
    response->add_data(0x00); // block size 1
    response->add_data(0x00); // block size 2
    response->add_data(0x00); // block size 3
    return response;
  }
  return nullptr;
}

std::unique_ptr<ControlResponse> FakeSmartPortHandler::control(ControlRequest* request) {
  // just print out the request, and return a successful response (status = 0)
  std::cout << "FakeSmartPortHandler::control called with request: " << std::endl;
  auto response = std::make_unique<ControlResponse>(request->get_request_sequence_number(), 0);
  return response;
}