#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "SLIPInterface.h"
#include "DeviceImpls/TCPDevice.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <address> <port>" << std::endl;
    return 1;
  }

  // Create a TCPDevice object on the specified port.
  TCPDevice device(argv[1], atoi(argv[2]));

  // Create a SLIPInterface object using the TCPDevice object.
  SLIPInterface slip_interface(std::make_unique<TCPDevice>(device));

  // Start a thread to listen for incoming SLIP packets.
  std::thread thread([&slip_interface]() {
    while (true) {
      // Read a SLIP packet.
      std::vector<uint8_t> packet = slip_interface.read();

      // Decode the SLIP packet.
      packet = slip_interface.decode(packet);
      std::string data(packet.begin(), packet.end());

      // Print the decoded data to the console.
      std::cout << data << std::endl;
    }
  });

  // Accept commands from the user.
  while (true) {
    std::string command;
    std::getline(std::cin, command);

    // If the command is "exit", terminate the application.
    if (command == "exit") {
      break;
    }

    // Encode the command into a SLIP packet.
    std::vector<uint8_t> packet;
    packet.assign(command.begin(), command.end());
    packet = slip_interface.encode(packet);

    // Write the SLIP packet to the device.
    slip_interface.write(packet);
  }

  // Wait for the listener thread to finish.
  thread.join();

  return 0;
}
