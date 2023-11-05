#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <string>
#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif
#include "Responder.h"

class Listener {
public:
  Listener(const std::string &ip_address, int port, std::unique_ptr<Responder> responder) 
    : ip_address_(ip_address), port_(port), is_listening_(false), responder_(std::move(responder)) {
      std::cout << "Creating Listener" << std::endl;
    }

  ~Listener();

  void start();
  void stop();

  std::thread createListenerThread();
  bool getIsListening() { return is_listening_; }

private:
  int port_;
  std::string ip_address_;
  std::unique_ptr<Responder> responder_;

  bool is_listening_;
  void handleConnection(int socket);
  void listenerFunction();
};
