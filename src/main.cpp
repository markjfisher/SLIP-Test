#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include <thread>
#include <condition_variable>
#include <mutex>
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

  std::condition_variable cv;
  std::mutex mtx;
  bool exit_flag = false;

  // Create a TCPDevice object on the specified port.
  TCPDevice device(argv[1], atoi(argv[2]));
  device.start_listener();

  // Create a SLIPInterface object using the TCPDevice object.
  SLIPInterface slip_interface(std::make_unique<TCPDevice>(device));

  // Start a thread to listen for incoming SLIP packets.
  std::thread thread([&slip_interface, &cv, &mtx, &exit_flag]() {
    while (true) {
      // std::cout << "creating lock" << std::endl;
      // std::unique_lock<std::mutex> lock(mtx);
      // std::cout << "waiting on lock" << std::endl;
      // cv.wait(lock, [&] { return exit_flag; });
      // std::cout << "ended wait" << std::endl;

      // if (exit_flag) {
      //   break;
      // }

      // Read a SLIP packet.
      std::vector<uint8_t> packet = slip_interface.read();

      if (!packet.empty()) {
        // Decode the SLIP packet.
        packet = slip_interface.decode(packet);
        std::string data(packet.begin(), packet.end());

        // Print the decoded data to the console.
        std::cout << data << std::endl;
      }
    }
  });

  std::cout << R"(
+-+ Mini SLIP v1.0.0 +-+

send <port> <message>  # send message to port
exit                   # quit application  
)";
  while (true) {
    std::string command;
    std::cout << "> ";
    std::getline(std::cin, command);

    // If the command is "exit", terminate the application.
    if (command == "exit") {
      std::cout << "exiting!" << std::endl;
      break;
    }

    if (command.find("send") == 0) {
      // Parse the port number and message from the command.
      std::string port_string = command.substr(5, command.find(" ") - 5);
      int port = std::stoi(port_string);
      std::string message = command.substr(command.find(" ") + 1);

      // Create a TCPDevice object on the specified port.
      TCPDevice target(argv[1], port);

      // Create a SLIPInterface object using the TCPDevice object.
      SLIPInterface slip_interface(std::make_unique<TCPDevice>(target));

      // Write the SLIP packet to the device.
      std::vector<uint8_t> packet(message.begin(), message.end());
      slip_interface.write(packet);
    }
  }

  // {
  //   std::unique_lock<std::mutex> lock(mtx);
  //   exit_flag = true;
  //   cv.notify_one();
  // }

  // // Wait for the thread to exit.
  // thread.join();

  return 0;
}
