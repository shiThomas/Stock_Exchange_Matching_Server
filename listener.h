#ifndef __LISTENER_H__
#define __LISTENER_H__
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#define MAXDATASIZE 65536
typedef struct addrinfo addrinfo;
typedef struct sockaddr_storage sockaddr_storage;
typedef struct sockaddr sockaddr;

class Listener {
private:
  const char *port;
  addrinfo host_info;
  //  int listen_fd;

public:
  int listen_fd;
  Listener();
  Listener(const char *p);
  ~Listener();
  int acceptRequest();
};
#endif
