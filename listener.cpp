#include "listener.h"

int Listener::acceptRequest() {
  sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof socket_addr;
  int newfd = accept(listen_fd, (sockaddr *)&socket_addr, &socket_addr_len);
  if (newfd == -1) {
    throw std::string("accept failed");
  }
  return newfd;
}

Listener::Listener(const char *p) : port(p) {
  signal(SIGPIPE, SIG_IGN);
  const char *hostname = NULL;
  addrinfo *host_info_list;

  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;
  try {
    if (getaddrinfo(hostname, port, &host_info, &host_info_list) != 0)
      throw std::string("getaddrinfo");
    listen_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (listen_fd == -1)
      throw std::string("socket");

    int yes = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    // set recv timeout
    if (bind(listen_fd, host_info_list->ai_addr, host_info_list->ai_addrlen) ==
        -1)
      throw std::string("bind");
    if (listen(listen_fd, SOMAXCONN) == -1)
      throw std::string("listen");
  } catch (std::string e) {
    freeaddrinfo(host_info_list);
    throw std::string("server init");
  }
  freeaddrinfo(host_info_list);
}

Listener::Listener() {
  signal(SIGPIPE, SIG_IGN);
  listen_fd = -1;
}
Listener::~Listener() {
  if (listen_fd != -1)
    close(listen_fd);
}
