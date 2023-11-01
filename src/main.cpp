#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include <thread>
#include <atomic>

#include "SLIPInterface.h"
#include "TCPDevice.h"

void hex_dump(std::vector<uint8_t> packet) {
  int offset = 0;
  int bytes_per_line = 16;

  while (offset < packet.size()) {
    std::cout << std::hex << offset << ": ";

    for (int i = 0; i < bytes_per_line && offset + i < packet.size(); i++) {
      std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)packet[offset + i] << " ";
    }

    std::cout << std::endl;

    offset += bytes_per_line;
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <address> <port>" << std::endl;
    return 1;
  }
  std::atomic_bool exit_flag = false;

  // Create a TCPDevice object on the specified port.
  TCPDevice device(argv[1], atoi(argv[2]));
  device.start_listener();

  // Create a SLIPInterface object using the TCPDevice object.
  SLIPInterface slip_interface(std::make_unique<TCPDevice>(device));

  // Start a thread to listen for incoming SLIP packets.
  std::thread thread([&slip_interface, &exit_flag]() {
    while (!exit_flag.load()) {
      // Read a SLIP packet.
      // std::cout << "thread: doing read" << std::endl;
      std::vector<uint8_t> packet = slip_interface.read();
      if (!packet.empty()) {
        // packet = slip_interface.decode(packet);
        std::string data(packet.begin(), packet.end());
        std::cout << "<<: [" << data << "]" << std::endl;
      }
      // std::cout << "thread: looping while" << std::endl;
    }
    std::cout << "Reading thread exiting" << std::endl;
  });

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
      // Create a TCPDevice object on the specified port.
      TCPDevice target(argv[1], port);

      // Create a SLIPInterface object using the TCPDevice object.
      SLIPInterface targetSlip(std::make_unique<TCPDevice>(target));

      // Write the SLIP packet to the device.
      std::vector<uint8_t> packet(message.begin(), message.end());
      targetSlip.write(packet);
    }

    std::cout << "> ";

  }
  std::cout << "killing read thread" << std::endl;
  exit_flag.store(true);
  thread.join();

  return 0;
}
