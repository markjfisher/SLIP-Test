#pragma once

#include <vector>
#include <memory>
#include <stdint.h>
#include "Connection.h"
#include "Request.h"
#include "Response.h"

class Requestor {
public:
  Requestor(Connection* connection);

  // The Request's deserialize function will always return a Response, e.g. StatusRequest -> StatusResponse
  std::unique_ptr<Response> Requestor::sendRequest(Request& request);

private:
  Connection* connection_;
};