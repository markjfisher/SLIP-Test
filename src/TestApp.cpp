
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <iomanip>
#include <thread>

#include "TestApp.h"

#include "Util.h"
#include "SLIP.h"
#include "TCPConnection.h"
#include "Listener.h"
#include "Requestor.h"
#include "Responder.h"
#include "StatusRequest.h"
#include "StatusResponse.h"
#include "FakeSmartPortHandler.h"

void TestApp::start_listener(std::string command) {
  std::istringstream iss(command);
  std::string word;
  iss >> word;  // Skip "start"

  std::string address;
  iss >> address; // read the host address

  int port;
  iss >> port;  // Read port number

  listener_ = std::make_unique<Listener>(address, port);
  listener_->start();
  std::cout << "Created listener on " << address << ":" << port << std::endl;
}

void TestApp::status(std::string device_id_string) {
  int device_id = std::stoi(device_id_string);

  Requestor requestor;

  StatusRequest status_request(10, static_cast<uint8_t>(device_id), 0);
  auto connection = listener_->find_connection_with_device(device_id);
  if (connection == nullptr) {
    std::cout << "No device found that could service unit " << static_cast<unsigned int>(device_id) << std::endl;
    return;
  }

  auto response = requestor.send_request(status_request, connection);
  StatusResponse* status_response = dynamic_cast<StatusResponse*>(response.get());
  if (status_response != nullptr) {
    std::cout << "Found valid status response:" << static_cast<unsigned int>(status_response->get_status()) << std::endl;
  } else {
    std::cout << "error casting to StatusResponse (probably timed out sending)" << std::endl;
  }
}

void TestApp::close_connection(int sock) {
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
}

void TestApp::connect_to_server(std::string command) {
  std::istringstream iss(command);
  std::string word;
  iss >> word;  // Skip "connect"

  std::string address;
  iss >> address; // read the host address

  int port;
  iss >> port;  // Read port number

  std::getline(iss, word);
  std::cout << "word: >" << word << "<" << std::endl;

  std::cout << "Attempting connection to " << address << ":" << port << std::endl;
  int sock;
#ifdef _WIN32
  WSADATA wsa_data;
  WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
#else
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
#endif
    std::cerr << "socket creation failed" << std::endl;
    return;
  }

  // Set up the server address
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
#ifdef _WIN32
  if (InetPton(AF_INET, TEXT(address.c_str()), &server_addr.sin_addr) <= 0) {
#else
  if (inet_pton(AF_INET, address.c_str(), &server_addr.sin_addr) <= 0) {
#endif
    std::cerr << "Failed to set server address: " << strerror(errno) << std::endl;
    close_connection(sock);
    return;
  }

  // Connect to the server
  if (connect(sock, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
    std::cerr << "Failed to connect to server: " << strerror(errno) << std::endl;
    close_connection(sock);
    return;
  }
  std::cout << "Connected to target" << std::endl;
  std::shared_ptr<Connection> conn = std::make_shared<TCPConnection>(sock);
  conn->set_is_connected(true);
  conn->create_read_channel();

  // Now create a smartport handler and responder for servicing requests.
  std::unique_ptr<SmartPortHandler> handler = std::make_unique<FakeSmartPortHandler>(word);

  // Create a Responder with the handler and the connection
  std::unique_ptr<Responder> responder = std::make_unique<Responder>(std::move(handler), conn);

  std::thread thread(&Responder::wait_for_requests, responder.get());
  thread.detach();

  responders_.push_back(std::move(responder));
}

void TestApp::shutdown() {
}

void TestApp::info() {
  if (listener_) {
    std::cout << listener_->to_string() << std::endl;
  } else {
    std::cout << "No listener" << std::endl;
  }
}