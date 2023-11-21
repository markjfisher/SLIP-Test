#pragma once

#include <memory>
#include "SmartPortCodes.h"

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

class SmartPortHandler {
public:
  virtual ~SmartPortHandler() = default;

  virtual std::unique_ptr<StatusResponse> status(StatusRequest* request) = 0;
  virtual std::unique_ptr<ReadBlockResponse> read_block(ReadBlockRequest* request) = 0;
  virtual std::unique_ptr<WriteBlockResponse> write_block(WriteBlockRequest* request) = 0;
  virtual std::unique_ptr<FormatResponse> format(FormatRequest* request) = 0;
  virtual std::unique_ptr<ControlResponse> control(ControlRequest* request) = 0;
  // virtual std::unique_ptr<Response> init(InitRequest* request) = 0;
  // virtual std::unique_ptr<Response> open(OpenRequest* request) = 0;
  // virtual std::unique_ptr<Response> close(CloseRequest* request) = 0;
  // virtual std::unique_ptr<Response> read(ReadRequest* request) = 0;
  // virtual std::unique_ptr<Response> write(WriteRequest* request) = 0;
  // virtual std::unique_ptr<Response> reset(ResetRequest* request) = 0;
};