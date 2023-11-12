#include <iostream>
#include <string>
#include <cstring>

#include "TestApp.h"

int main(int argc, char* argv[]) {
  TestApp app;
  std::cout << R"(
+-+ SP over SLIP tester v1.0.0 +-+

# START LISTENER (this is equivalent of AppleWin, the owner of the listener)
start address port     # create listener service on address:port given

# CONNECT TO LISTENER (equivalent of devices attaching to device needed by listener)
# create a connection with at hostname/port, where data is "1,NETWORK,2,..." etc
# each number is a local unit ID and a "name" of the device. Only the ID is used in this test app.
connect hostname port data

# SEND SP COMMANDS
# These are issued from the LISTENER instance of the application
# NOTE: device_ids are enumerated 1, 2, 3, ... in the LISTENER, but the target id may be different.
# so that if TWO connections are made, and each thinks its ID is "1", the LISTENER will be able to differentiate them
status device_id       # send status SP request for device id.

# DISPLAY INFO ABOUT LISTENER
# this will show the Listener object, and enumerate all the devices attached, showing their DEVICE ID to the listener (and their unit IDs on the device)
info                   # display listener info

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
      app.start_listener(command);
    }

    if (command.find("connect") == 0) {
      app.connect_to_server(command);
    }

    if (command.find("status") == 0) {
      app.check_status(command);
    }

    if (command.find("info") == 0) {
      app.info();
    }

    std::cout << "> ";

  }
  return 0;
}