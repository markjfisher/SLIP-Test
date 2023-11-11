#pragma once

#include <memory>
#include <string>

#include "Listener.h"
#include "FakeSmartPortHandler.h"
#include "Responder.h"

class TestApp {
private:
  std::unique_ptr<Listener> listener_;
  // std::vector<int> sockets_;
  std::vector<std::unique_ptr<Responder>> responders_;

  void closeConnection(int sock);

public:
  void startListener(std::string command);
  void checkStatus(std::string command);
  void connectToServer(std::string command);
  // void listDevices();
  // void listConnections();
  void info();
  void shutdown();

};