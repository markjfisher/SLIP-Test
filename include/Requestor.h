#pragma once

#include <memory>
#include "Connection.h"
#include "Request.h"
#include "Response.h"

class Requestor {
public:
  Requestor(Listener* listener) : listener_(listener) {}

  // The Request's deserialize function will always return a Response, e.g. StatusRequest -> StatusResponse
  std::unique_ptr<Response> sendRequest(const Request& request);

private:
  Listener* listener_;
};