#pragma once

#include <memory>
#include "ControlRequest.h"
#include "ControlResponse.h"
#include "StatusRequest.h"
#include "StatusResponse.h"

#include "SmartPortCodes.h"

class SmartPortHandler {
public:
  virtual ~SmartPortHandler() = default;

  virtual std::unique_ptr<StatusResponse> status(StatusRequest* request) = 0;
  // virtual std::unique_ptr<Response> read_block(ReadBlockRequest* request) = 0;
  // virtual std::unique_ptr<Response> write_block(WriteBlockRequest* request) = 0;
  // virtual std::unique_ptr<Response> format(FormatRequest* request) = 0;
  virtual std::unique_ptr<ControlResponse> control(ControlRequest* request) = 0;
  // virtual std::unique_ptr<Response> init(InitRequest* request) = 0;
  // virtual std::unique_ptr<Response> open(OpenRequest* request) = 0;
  // virtual std::unique_ptr<Response> close(CloseRequest* request) = 0;
  // virtual std::unique_ptr<Response> read(ReadRequest* request) = 0;
  // virtual std::unique_ptr<Response> write(WriteRequest* request) = 0;
  // virtual std::unique_ptr<Response> reset(ResetRequest* request) = 0;
};