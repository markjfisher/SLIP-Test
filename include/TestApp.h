#pragma once

#include <memory>
#include <string>

#include "Listener.h"
#include "FakeSmartPortHandler.h"

class TestApp {
private:
  std::unique_ptr<FakeSmartPortHandler> handler_;
  std::unique_ptr<Listener> listener_;

  std::vector<int> sockets_;

  void closeConnection(int sock);

public:
  void startListener(std::string command);
  void checkStatus(std::string command);
  void connectToServer(std::string command);
  void shutdown();

};