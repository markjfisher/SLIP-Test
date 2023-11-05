#pragma once

#include <memory>
#include "Connection.h"
#include "Request.h"
#include "Response.h"

class Requestor {
public:
  Requestor(std::unique_ptr<Connection> connection);

  // The Request's deserialize function will always return a Response, e.g. StatusRequest -> StatusResponse
  std::unique_ptr<Response> sendRequest(const Request& request);

private:
  std::unique_ptr<Connection> connection_;
};