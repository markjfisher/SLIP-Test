#include <sstream>

#include "FakeSmartPortHandler.h"
#include "Util.h"

#include "CloseRequest.h"
#include "CloseResponse.h"
#include "ControlRequest.h"
#include "ControlResponse.h"
#include "FormatRequest.h"
#include "FormatResponse.h"
#include "InitRequest.h"
#include "InitResponse.h"
#include "OpenRequest.h"
#include "OpenResponse.h"
#include "ReadBlockRequest.h"
#include "ReadBlockResponse.h"
#include "ReadRequest.h"
#include "ReadResponse.h"
#include "ResetRequest.h"
#include "ResetResponse.h"
#include "StatusRequest.h"
#include "StatusResponse.h"
#include "WriteBlockRequest.h"
#include "WriteBlockResponse.h"
#include "WriteRequest.h"
#include "WriteResponse.h"


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
  auto data = request->serialize();
  Util::hex_dump(data);

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
  std::cout << "FakeSmartPortHandler::control" << std::endl;
  auto data = request->serialize();
  Util::hex_dump(data);
  auto response = std::make_unique<ControlResponse>(request->get_request_sequence_number(), 0);
  return response;
}

std::unique_ptr<ReadBlockResponse> FakeSmartPortHandler::read_block(ReadBlockRequest* request) {
  // Return a block of 512 bytes, data will contain a string of the block number, nul marker, then 0xAA filling rest
  std::cout << "FakeSmartPortHandler::read_block" << std::endl;
  auto data = request->serialize();
  Util::hex_dump(data);
  auto response = std::make_unique<ReadBlockResponse>(request->get_request_sequence_number(), 0);
  // Fill the data block with some values. Create a string out of the block number
  uint32_t number = (static_cast<uint32_t>(request->get_block_number()[2]) << 16) |
                  (static_cast<uint32_t>(request->get_block_number()[1]) << 8) |
                  static_cast<uint32_t>(request->get_block_number()[0]);
  std::string str = std::to_string(number);
  std::vector<uint8_t> return_data(512, 0xAA);
  std::copy(str.begin(), str.end(), return_data.begin());
  return_data[str.size()] = '\0';
  response->set_block_data(return_data.begin(), return_data.end());

  return response;
}

std::unique_ptr<WriteBlockResponse> FakeSmartPortHandler::write_block(WriteBlockRequest* request) {
  std::cout << "FakeSmartPortHandler::write_block" << std::endl;
  auto data = request->serialize();
  Util::hex_dump(data);
  auto response = std::make_unique<WriteBlockResponse>(request->get_request_sequence_number(), 0);
  return response;
}

std::unique_ptr<FormatResponse> FakeSmartPortHandler::format(FormatRequest* request) {
  std::cout << "FakeSmartPortHandler::format" << std::endl;
  auto data = request->serialize();
  Util::hex_dump(data);
  auto response = std::make_unique<FormatResponse>(request->get_request_sequence_number(), 0);
  return response;
}

