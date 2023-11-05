#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include <thread>

#include "TCPConnection.h"
#include "Listener.h"
#include "Requestor.h"
#include "StatusRequest.h"
#include "StatusResponse.h"
#include "FakeSmartPortHandler.h"

int main(int argc, char* argv[]) {
  std::unique_ptr<FakeSmartPortHandler> handler;
  std::unique_ptr<Responder> responder;
  std::unique_ptr<Listener> listener;

  std::cout << R"(
+-+ SP over SLIP tester v1.0.0 +-+

start <port>      # create listener on port given
status <port>     # send status request to port

exit              # exit application
)";
  std::string command;
  std::cout << "> ";
  while (std::getline(std::cin, command)) {
    if (command == "exit") {
      break;
    }

    if (command.find("start") == 0) {
      handler = std::make_unique<FakeSmartPortHandler>();
      responder = std::make_unique<Responder>(std::move(handler));

      std::string port_string = command.substr(7, command.find(" ") - 7);
      int port = std::stoi(port_string);
      listener = std::make_unique<Listener>("127.0.0.1", port, std::move(responder));
      listener->start();
      std::cout << "Created listener" << std::endl;
    }

    if (command.find("status") == 0) {
      std::string port_string = command.substr(8, command.find(" ") - 8);
      int port = std::stoi(port_string);

      std::unique_ptr<TCPConnection> connection = std::make_unique<TCPConnection>("127.0.0.1", port);
      Requestor requestor(std::move(connection));

      StatusRequest statusRequest(10, 1, 0);
      auto response = requestor.sendRequest(statusRequest);
      StatusResponse* statusResponse = dynamic_cast<StatusResponse*>(response.get());
      if (statusResponse != nullptr) {
        std::cout << "Found valid status response:" << statusResponse->get_status() << std::endl;
      } else {
        std::cout << "error casting to StatusResponse" << std::endl;
      }
    }

    std::cout << "listening still? " << listener->getIsListening() << std::endl;
    std::cout << "> ";

  }
  std::cout << "Tumbling out of main" << std::endl;
  return 0;
}
