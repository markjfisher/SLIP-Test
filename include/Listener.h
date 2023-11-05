#pragma once

#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif


class Listener {
public:
  Listener(const std::string &ip_address, int port) : ip_address_(ip_address), port_(port), is_listening_(false) {}
  ~Listener();

  void start();
  void stop();

  std::vector<uint8_t> Listener::getPacket(uint8_t index);
  void addPacket(uint8_t index, const std::vector<uint8_t>& packet);
  bool hasResponse(uint8_t sequence_number) const;

  std::thread createListenerThread();
  void startPruning();
  void stopPruning();

private:
  int port_;
  std::string ip_address_;

  bool is_listening_;
  bool is_pruning_;

  std::thread pruning_thread_;
  std::unordered_map<uint8_t, std::pair<std::chrono::system_clock::time_point, std::vector<uint8_t>>> response_map_;

  // remove entries from the response map that are older than max_age.
  void prune(std::chrono::system_clock::duration max_age);

  void listenerFunction();
};

