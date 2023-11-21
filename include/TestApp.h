#pragma once

#include <memory>
#include <string>

#include "Listener.h"
#include "FakeSmartPortHandler.h"
#include "Responder.h"

class TestApp {
private:
  std::unique_ptr<Listener> listener_;
  std::vector<std::unique_ptr<Responder>> responders_;

  void close_connection(int sock);

public:
  void start_listener(std::string command);
  void status(std::string command);
  void connect_to_server(std::string command);
  void info();
  void shutdown();
};