#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include <thread>
#include <atomic>

#include "Connection.h"
#include "Listener.h"
#include "Requestor.h"

int main(int argc, char* argv[]) {
  std::atomic_bool exit_flag = false;

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

    if (command.find("status") == 0) {
      // Parse the port number and message from the command.
      std::string port_string = command.substr(7, command.find(" ") - 7);
      int port = std::stoi(port_string);

    }

    std::cout << "> ";

  }
  std::cout << "killing read thread" << std::endl;
  exit_flag.store(true);

  return 0;
}
