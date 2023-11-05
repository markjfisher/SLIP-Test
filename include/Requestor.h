#pragma once

#include <vector>
#include <stdint.h>
#include "Connection.h"
#include "Listener.h"

#define REQUESTOR_TIMEOUT (30)

class Requestor {
public:
  Requestor(Connection* connection, Listener* listener);

  // The Request's deserialize function will always return a Response, e.g. StatusRequest -> StatusResponse
  Response* Requestor::sendRequest(Request& request);

private:

  Listener* listener_;
  Connection* connection_;

  std::unordered_map<uint8_t, Request> sent_requests_;
  std::mutex mtx_;
  std::condition_variable cv_;
};