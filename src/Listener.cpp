#include "Listener.h"
#include "SLIP.h"
#include <iostream>

Listener::~Listener() {
  stop();
}

std::thread Listener::createListenerThread() {
  return std::thread(&Listener::listenerFunction, this);
}

void Listener::listenerFunction() {
    std::thread listener_thread([this]() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
      std::cerr << "socket creation failed" << std::endl;
    }
#else
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }
#endif

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    address.sin_addr.s_addr = inet_addr(ip_address_.c_str());

#ifdef _WIN32
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
      std::cerr << "bind failed" << std::endl;
    }
#else
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }
#endif

    if (listen(server_fd, 3) < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
    }

    while (is_listening_) {
#ifdef _WIN32
      if ((new_socket = accept(server_fd, (SOCKADDR*)&address, &addrlen)) == INVALID_SOCKET) {
        perror("accept");
        exit(EXIT_FAILURE);
      }
#else
      if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }
#endif

      std::thread([this, new_socket]() {
        handleConnection(new_socket);
#ifdef _WIN32
        closesocket(new_socket);
#else
        close(new_socket);
#endif
      }).detach();

    }

#ifdef _WIN32
    closesocket(server_fd);
    WSACleanup();
#else
    close(server_fd);
#endif
  });

}

// This accepts Request operations, and returns Response data on the same socket.
void Listener::handleConnection(int socket) {
  std::vector<uint8_t> completeData;
  std::vector<uint8_t> buffer(1024);

  int valread = 0;
  do {
#ifdef _WIN32
    valread = recv(socket, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);
#else
    valread = read(socket, buffer.data(), buffer.size());
#endif
    if (valread > 0) {
      completeData.insert(completeData.end(), buffer.begin(), buffer.begin() + valread);
    }
  } while (valread > 0);

  // for every slip packet we received (probably only 1 as this is a 1:1 request to response) process it and send back the response.
  if (!completeData.empty()) {
    std::vector<std::vector<uint8_t>> packets = SLIP::splitIntoPackets(completeData.data(), completeData.size());
    for (const auto& packet : packets) {
      if (!packet.empty()) {
        std::vector<uint8_t> response = SLIP::encode(responder_->process(packet));

#ifdef _WIN32
        send(socket, reinterpret_cast<const char *>(response.data()), response.size(), 0);
#else
        write(socket, response.data(), response.size());
#endif

      }
    }
  }
  else {
    // handle no data?
  }
}

void Listener::start() {
  is_listening_ = true;
  createListenerThread().detach();
}

void Listener::stop() {
  is_listening_ = false;
}
