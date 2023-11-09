#include <iostream>
#include <string>
#include <cstring>

#include "TestApp.h"

int main(int argc, char* argv[]) {
  TestApp app;
  std::cout << R"(
+-+ SP over SLIP tester v1.0.0 +-+

start port             # create listener service on port given

# create a connection with at hostname/port with name, where data is "1,NETWORK,2,..." etc
connect hostname port name data

status unit            # send status request to listener for SP unit

devices                # list devices connected
connections            # list connections

exit              # exit application
)";
  std::string command;
  std::cout << "> ";
  while (std::getline(std::cin, command)) {
    if (command == "exit") {
      app.shutdown();
      break;
    }

    // add 2 to string length for the next arg
    if (command.find("start") == 0) {
      app.startListener(command);
    }

    if (command.find("connect") == 0) {
      app.connectToServer(command);
    }

    if (command.find("status") == 0) {
      app.checkStatus(command);
    }

    std::cout << "> ";

  }
  std::cout << "Tumbling out of main" << std::endl;
  return 0;
}