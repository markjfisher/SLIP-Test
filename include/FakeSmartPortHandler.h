#pragma once

#include <map>
#include <memory>
#include <string>
#include <iostream>
#include "SmartPortHandler.h"

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


class FakeSmartPortHandler : public SmartPortHandler {

public:
  FakeSmartPortHandler(std::string capabilities) {
    parse_capabilities(capabilities);
    std::cout << "Creating FakeSmartPortHandler, capabilities: " << capabilities << ", size: " << capability_map_.size() << std::endl;
  }

  virtual std::unique_ptr<StatusResponse> status(StatusRequest* request) override;
  virtual std::unique_ptr<ReadBlockResponse> read_block(ReadBlockRequest* request) override;
  virtual std::unique_ptr<WriteBlockResponse> write_block(WriteBlockRequest* request) override;
  virtual std::unique_ptr<FormatResponse> format(FormatRequest* request) override;
  virtual std::unique_ptr<ControlResponse> control(ControlRequest* request) override;
  // virtual std::unique_ptr<XResponse> init(InitRequest* request) override;
  // virtual std::unique_ptr<XResponse> open(OpenRequest* request) override;
  // virtual std::unique_ptr<XResponse> close(CloseRequest* request) override;
  // virtual std::unique_ptr<XResponse> read(ReadRequest* request) override;
  // virtual std::unique_ptr<XResponse> write(WriteRequest* request) override;
  // virtual std::unique_ptr<XResponse> reset(ResetRequest* request) override;

  void parse_capabilities(const std::string& input);

private:
  std::map<int, std::string> capability_map_;

};