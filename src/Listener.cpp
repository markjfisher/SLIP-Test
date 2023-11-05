#include "Listener.h"
#include "SLIP.h"
#include <iostream>

Listener::~Listener() {
  if (is_pruning_) {
    stopPruning();
  }
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

      std::vector<uint8_t> completeData;
      std::vector<uint8_t> buffer(1024);

      int valread = 0;
      do {
#ifdef _WIN32
        valread = recv(new_socket, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);
#else
        valread = read(new_socket, buffer.data(), buffer.size());
#endif
        if (valread > 0) {
          completeData.insert(completeData.end(), buffer.begin(), buffer.begin() + valread);
        }
      } while (valread > 0);

      if (!completeData.empty()) {
        std::vector<std::vector<uint8_t>> packets = SLIP::splitIntoPackets(completeData.data(), completeData.size());
        for (const auto& packet : packets) {
          if (!packet.empty()) {
            response_map_[packet[0]] = std::make_pair(std::chrono::system_clock::now(), packet);
          }
        }
      }
      else {
          // handle error
      }

#ifdef _WIN32
      closesocket(new_socket);
#else
      close(new_socket);
#endif
    }

#ifdef _WIN32
    closesocket(server_fd);
    WSACleanup();
#else
    close(server_fd);
#endif
  });

}

void Listener::start() {
  is_listening_ = true;
  createListenerThread().detach();
  startPruning();
}

void Listener::stop() {
  is_listening_ = false;
}

std::pair<std::chrono::system_clock::time_point, std::vector<uint8_t>> Listener::getPacket(uint8_t index) {
  if (response_map_.find(index) != response_map_.end()) {
    return response_map_[index];
  } else {
    return { std::chrono::system_clock::time_point(), std::vector<uint8_t>() };  // return empty pair
  }
}

void Listener::addPacket(uint8_t index, const std::vector<uint8_t>& packet) {
  response_map_[index] = std::make_pair(std::chrono::system_clock::now(), packet);
}

void Listener::prune(std::chrono::system_clock::duration max_age) {
  auto now = std::chrono::system_clock::now();

  for (auto it = response_map_.begin(); it != response_map_.end(); ) {
    auto age = now - it->second.first;
    if (age > max_age) {
      it = response_map_.erase(it);
    } else {
      ++it;
    }
  }
}

void Listener::startPruning() {
  is_pruning_ = true;
  pruning_thread_ = std::thread([this] {
    while (is_pruning_) {
      prune(std::chrono::minutes(30));
      std::this_thread::sleep_for(std::chrono::minutes(1));
    }
  });
}

void Listener::stopPruning() {
  is_pruning_ = false;
  if (pruning_thread_.joinable()) {
    pruning_thread_.join();
  }
}