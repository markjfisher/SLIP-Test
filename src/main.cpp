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
  std::unique_ptr<Listener> listener;

  std::cout << R"(
+-+ SP over SLIP tester v1.0.0 +-+

start <port>      # create listener service on port given
connect <port>    # create a connection to the service
status <unit>     # send status request to connection
devices           # list devices connected

exit              # exit application
)";
  std::string command;
  std::cout << "> ";
  while (std::getline(std::cin, command)) {
    if (command == "exit") {
      break;
    }

    // add 2 to string length for the next arg
    if (command.find("start") == 0) {
      std::string port_string = command.substr(7, command.find(" ") - 7);
      int port = std::stoi(port_string);
      listener = std::make_unique<Listener>("127.0.0.1", port);
      listener->start();
      std::cout << "Created listener" << std::endl;
    }

    if (command.find("connect") == 0) {
      std::string port_string = command.substr(9, command.find(" ") - 9);
      int port = std::stoi(port_string);

      // connect to the listener, and send the capabilities of the fake SP device
      

    }

    if (command.find("status") == 0) {
      std::string unit_string = command.substr(8, command.find(" ") - 8);
      int unit = std::stoi(unit_string);

      Requestor requestor(listener.get());

      StatusRequest statusRequest(10, 1, 0);
      auto response = requestor.sendRequest(statusRequest);
      StatusResponse* statusResponse = dynamic_cast<StatusResponse*>(response.get());
      if (statusResponse != nullptr) {
        std::cout << "Found valid status response:" << statusResponse->get_status() << std::endl;
      } else {
        std::cout << "error casting to StatusResponse" << std::endl;
      }
    }

    std::cout << "> ";

  }
  std::cout << "Tumbling out of main" << std::endl;
  return 0;
}
