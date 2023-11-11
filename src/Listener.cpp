#include <atomic>
#include <cstring>

#include "Listener.h"
#include "TCPConnection.h"
#include "SLIP.h"
#include "Util.h"

std::atomic<bool> stopListenerThread = false;

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
    exit(EXIT_FAILURE);
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

// Creates a Connecton object, which is how a SP will register itself with our listener.
// When it connects, it tells us all the available unit IDs and names available to service requests.
// That connection will then be used to send requests to, and get responses back from.
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
    std::cout << "Listener::createConnection, recieved " << valread << " bytes" << std::endl;
    if (valread > 0) {
      completeData.insert(completeData.end(), buffer.begin(), buffer.begin() + valread);
    }
  } while (valread == 1024);

  int cdSize = static_cast<unsigned int>(completeData.size());
  std::cout << "capability data from incoming connection:" << std::endl;
  Util::hex_dump(completeData);

  // for every slip packet we received (probably only 1 as this is an initial connection) 
  // which are pairs of int, c-string (ID, Name (0 terminated))
  if (!completeData.empty()) {
    std::vector<std::vector<uint8_t>> packets = SLIP::splitIntoPackets(completeData.data(), completeData.size());
    std::cout << "packets size: " << packets.size() << ", completeData size: " << completeData.size() << std::endl;

    if (!packets.empty()) {
      // We have at least some data that might pass as a capability, let's create the Connection object, and use it to parse the data
      std::shared_ptr<Connection> conn = std::make_shared<TCPConnection>(socket);
      for (const auto& packet : packets) {
        std::cout << ".. packet:" << std::endl;
        Util::hex_dump(packet);
        if (!packet.empty()) {
          std::cout << ".. adding devices" << std::endl;
          // create devices from the data
          conn->addDevices(packet);
          std::cout << ".. added devices" << std::endl;
        }
      }
      if (!conn->getDevices().empty()) {
        std::cout << ".. found devices, setting connected, then adding to connections" << std::endl;
        // this connection is a keeper! it has some devices on it.
        conn->setIsConnected(true);
        std::cout << ".. creating read channel" << std::endl;
        conn->createReadChannel();
        std::cout << ".. read channel created" << std::endl;
        // create a closure, so the mutex is released as it goes out of scope
        {
          std::cout << ".. getting lock" << std::endl;
          std::lock_guard<std::mutex> lock(mtx_);
          std::cout << ".. got it, adding to connections_" << std::endl;
          connections_.push_back(conn);
        } // mtx_ unlocked here
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

std::string Listener::toString() {
  std::stringstream ss;
  ss << "Listener: ip_address = " << ip_address_ << ", port = " << port_ << ", is_listening = " << (is_listening_ ? "true" : "false") << ", connections = [";
  for (const auto& connection : connections_) {
    ss << connection->toString() << ", ";
  }
  std::string str = ss.str();
  if (!connections_.empty()) {
    // Remove the last comma and space
    str = str.substr(0, str.size() - 2);
  }
  str += "]";
  return str;
}
