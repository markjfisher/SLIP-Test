#include "Listener.h"
#include "TCPConnection.h"
#include "SLIP.h"
#include "Util.h"

Listener::~Listener() {
  std::cout << "In listener destructor" << std::endl;
  stop();
}

std::thread Listener::createListenerThread() {
  return std::thread(&Listener::listenerFunction, this);
}

void Listener::listenerFunction() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  std::cout << "In listenerFunction, creating socket" << std::endl;
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

  std::cout << "... binding" << std::endl;
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

  std::cout << "... listening" << std::endl;
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  while (is_listening_) {
    std::cout << "... accepting connection" << std::endl;

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

    // do it in a thread so we can respond to more requests as soon as possible
    std::thread([this, new_socket]() {
      createConnection(new_socket);
    }).detach();

  }

  std::cout << "Listener closing down." << std::endl;

#ifdef _WIN32
  closesocket(server_fd);
  WSACleanup();
#else
  close(server_fd);
#endif

}

// This accepts Request operations, and returns Response data on the same socket.
void Listener::createConnection(int socket) {
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

  std::cout << "capability data from incoming connection:" << std::endl;
  Util::hex_dump(completeData);

  // for every slip packet we received (probably only 1 as this is an initial connection giving us details of capabilities of SP service), get the capabilities, in form of list of Units
  // which are pairs of int, c-string (ID, Name (0 terminated))
  if (!completeData.empty()) {
    std::vector<std::vector<uint8_t>> packets = SLIP::splitIntoPackets(completeData.data(), completeData.size());

    if (!packets.empty()) {
      // We have at least some data that might pass as a capability, let's create the Connection object, and use it to parse the data
      std::unique_ptr<Connection> conn = std::make_unique<TCPConnection>(socket);
      for (const auto& packet : packets) {
        if (!packet.empty()) {
          // create devices from the data
          conn->addDevices(packet);
        }
      }
      if (!conn->getDevices().empty()) {
        // this connection is a keeper! it has some devices on it.
        std::lock_guard<std::mutex> lock(mtx_);
        connections_.push_back(std::move(conn));
      }
    }
  }
}

void Listener::start() {
  is_listening_ = true;
  std::thread(&Listener::listenerFunction, this).detach();
}

void Listener::stop() {
  is_listening_ = false;
}

Connection* Listener::findConnectionWithDevice(int deviceId) {
  for (const auto& connection : connections_) {
    if (connection->findDevice(deviceId) != nullptr) {
      return connection.get();
    }
  }
  return nullptr;
}