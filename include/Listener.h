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
  : ip_address_(ip_address), port_(port), is_listening_(false) {}

  ~Listener();

  void start();
  void stop();

  std::thread create_listener_thread();
  bool get_is_listening() { return is_listening_; }

  Connection* find_connection_with_device(int device_id);

  std::string to_string();

private:
  std::string ip_address_;
  int port_;
  std::mutex mtx_;
  std::vector<std::shared_ptr<Connection>> connections_;

  bool is_listening_;
  void create_connection(int socket);
  void listener_function();
};
