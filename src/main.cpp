#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include "TestApp.h"


inline constexpr auto hash_djb2a(const std::string_view sv) {
    unsigned long hash{ 5381 };
    for (unsigned char c : sv) {
        hash = ((hash << 5) + hash) ^ c;
    }
    return hash;
}

inline constexpr auto operator"" _sh(const char *str, size_t len) {
    return hash_djb2a(std::string_view{ str, len });
}

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
  std::string firstWord;
  std::string arg1;
  std::cout << "> ";
  bool exiting = false;
  while (!exiting && std::getline(std::cin, command)) {
    std::istringstream iss(command);
    iss >> firstWord;
    iss >> arg1;
    switch (hash_djb2a(firstWord)) {

    case "exit"_sh:
      app.shutdown();
      exiting = true;
      break;

    case "start"_sh:
      app.start_listener(command);
      break;

    case "info"_sh:
      app.info();
      break;

    case "connect"_sh:
      app.connect_to_server(command);
      break;

    // THESE ARE FOR GENERATING A REQUEST TO A TARGET CONNECTION
    // This was done when initially testing the application without AppleWin involved.
    // They could be implemented again, but are re-implementations of the FujiNet::process_sp_over_slip() code in AppleWin.
    // Instead, look at the Responder class, which this client is acting as for most new functions,
    // with the requestor running in AppleWin and invoked through 6502 applications.

    case "status"_sh:
      app.status(arg1);
      break;

    // TODO if needed:
    // case "read_block"_sh:
    //   app.read_block(arg1);
    //   break;

    }

    std::cout << "> ";

  }
  return 0;
}