#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include <thread>
#include <atomic>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <address> <port>" << std::endl;
    return 1;
  }
  std::atomic_bool exit_flag = false;

  std::cout << R"(
+-+ Mini SLIP v1.0.0 +-+

send <port> <message>  # send message to port
exit                   # exit application
)";
  std::string command;
  std::cout << "> ";
  while (std::getline(std::cin, command)) {
    // If the command is "exit", terminate the application.
    if (command == "exit") {
      std::cout << "exiting!" << std::endl;
      break;
    }

    if (command.find("send") == 0) {
      // Parse the port number and message from the command.
      std::string port_string = command.substr(5, command.find(" ") - 5);
      int port = std::stoi(port_string);

      // Find the first space character after the port number.
      size_t message_start = command.find(" ", command.find(" ") + 1);
      std::string message;
      if (message_start != std::string::npos) {
        // Extract the message from the command.
        message = command.substr(message_start + 1);
      } else {
        // No message specified.
        message = "Wassup?";
      }
      std::cout << "sending: " << message << std::endl;
    }

    std::cout << "> ";

  }
  std::cout << "killing read thread" << std::endl;
  exit_flag.store(true);

  return 0;
}
