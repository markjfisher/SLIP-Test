#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
 #include <winsock2.h>
 #pragma comment(lib, "ws2_32.lib")
 #include <ws2tcpip.h>
#else
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <unistd.h>
#endif

#include "Connection.h"

class Listener {
public:
  Listener(const std::string &ip_address, int port) 
  : ip_address_(ip_address), port_(port), is_listening_(false) {
      std::cout << "Creating Listener" << std::endl;
    }

  ~Listener();

  void start();
  void stop();

  std::thread createListenerThread();
  bool getIsListening() { return is_listening_; }

  Connection* findConnectionWithDevice(int deviceId);

  std::string toString();

private:
  std::string ip_address_;
  int port_;
  std::mutex mtx_;
  std::vector<std::shared_ptr<Connection>> connections_;

  bool is_listening_;
  void createConnection(int socket);
  void listenerFunction();
};
