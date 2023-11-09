
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <thread>
#include <cstring>

#include "TestApp.h"

#include "Util.h"
#include "SLIP.h"
#include "TCPConnection.h"
#include "Listener.h"
#include "Requestor.h"
#include "StatusRequest.h"
#include "StatusResponse.h"
#include "FakeSmartPortHandler.h"

void TestApp::startListener(std::string command) {
  std::string port_string = command.substr(6);
  int port = std::stoi(port_string);
  listener_ = std::make_unique<Listener>("127.0.0.1", port);
  listener_->start();
  std::cout << "Created listener on port " << port_string << std::endl;
}

void TestApp::checkStatus(std::string command) {
  std::string unit_string = command.substr(7);
  std::cout << "unit_string: >" << unit_string << "<" << std::endl;
  int unit = std::stoi(unit_string);

  Requestor requestor(listener_.get());

  StatusRequest statusRequest(10, 1, 0);
  auto response = requestor.sendRequest(statusRequest);
  StatusResponse* statusResponse = dynamic_cast<StatusResponse*>(response.get());
  if (statusResponse != nullptr) {
    std::cout << "Found valid status response:" << statusResponse->get_status() << std::endl;
  } else {
    std::cout << "error casting to StatusResponse" << std::endl;
  }
}

void TestAPp::closeConnection(int sock) {
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
}

void TestApp::connectToServer(std::string command) {
  std::istringstream iss(command);
  std::string word;
  iss >> word;  // Skip "connect"

  std::string address;
  iss >> address; // read the host address

  int port;
  iss >> port;  // Read port number

  std::string name;
  iss >> name;  // Read connection "name"

  std::getline(iss, word, ' ');  // Skip space

  std::vector<uint8_t> data;
  while (std::getline(iss, word, ',')) {
    // Convert ID to uint8_t and add to data
    int id = std::stoi(word);
    data.push_back(static_cast<uint8_t>(id));

    // Get name and add to data
    std::getline(iss, word, ',');
    for (char c : word) {
      data.push_back(static_cast<uint8_t>(c));
    }
    data.push_back('\0');  // Add null terminator
  }

  // Now data contains the parsed data

  std::cout << "Attempting connection to " << address << ":" << port << std::endl;
  int sock;
#ifdef _WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
#else
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
#endif
    std::cerr << "socket creation failed" << std::endl;
    return;
  }

  // Set up the server address, this is only for testing, hence using 127.0.0.1
  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
#ifdef _WIN32
  if (InetPton(AF_INET, TEXT(address), &serverAddr.sin_addr) <= 0) {
#else
  if (inet_pton(AF_INET, address, &serverAddr.sin_addr) <= 0) {
#endif
    std::cerr << "Failed to set server address: " << strerror(errno) << std::endl;
    closeConnection(sock);
    return;
  }

  // Connect to the server
  if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
    std::cerr << "Failed to connect to server: " << strerror(errno) << std::endl;
    closeConnection(sock);
    return;
  }

  // Send the data
  auto slip_data = SLIP::encode(data);
#ifdef _WIN32
  auto res = send(sock, reinterpret_cast<const char *>(slip_data.data()), slip_data.size(), 0);
#else
  auto res = write(sock, slip_data.data(), slip_data.size());
#endif

  if (res < 0) {
    std::cerr << "Failed to send data: " << strerror(errno) << std::endl;
    closeConnection();
    return;
  }

  sockets_.push_back(sock);
  // TODO: Create the ReadChannel on client side, and react to packets. See TCPConnection::createReadChannel

}


void TestApp::shutdown() {

}