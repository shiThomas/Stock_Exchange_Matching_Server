#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
using namespace std;

int status;
int master_fd;
int len;

struct sockaddr_in newplayer, master_sock;
char host[64], str[64];
char buffer[40960], temp[40960];
struct hostent *master_host_addr;
socklen_t newplayer_addr_len = sizeof(newplayer);
/*
void set_server(char host[64], const char * port, int port_num) {
  //set address and port
  master_sock.sin_family = AF_INET;
  master_sock.sin_port = htons(port_num);

  memcpy(&master_sock.sin_addr, master_host_addr->h_addr_list[0],
         master_host_addr->h_length);

  master_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (master_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    exit(-1);
  } // if

  int yes = 1;
  status = setsockopt(master_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status =
      bind(master_fd, (struct sockaddr *)&master_sock, sizeof(master_sock));
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    exit(-1);
  } // if

  status = listen(master_fd, 5);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    exit(-1);
  } // if

  // cout << "Waiting for connection on port " << port << endl;
}
*/
int main(int argc, char *argv[]) {
  srand(time(NULL));
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port_num> \n", argv[0]);
    exit(1);
  }
  int port_num = atoi(argv[1]);
  // Initial Erro Checking

  if (port_num < 0) {
    printf("Invalid Port Numebr\n");
    return 0;
  }

  const char *port = argv[1];

  gethostname(host, sizeof(host));
  master_host_addr = gethostbyname(host);

  if (master_host_addr == NULL) {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }

  // setserver
  // set address and port
  master_sock.sin_family = AF_INET;
  master_sock.sin_port = htons(port_num);

  memcpy(&master_sock.sin_addr, master_host_addr->h_addr_list[0],
         master_host_addr->h_length);

  master_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (master_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    exit(-1);
  } // if

  int yes = 1;
  status = setsockopt(master_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status =
      bind(master_fd, (struct sockaddr *)&master_sock, sizeof(master_sock));
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    exit(-1);
  } // if

  status = listen(master_fd, 5);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    exit(-1);
  } // if
  // end of setserver

  // set_server(host, port, port_num);
  int listen_fd =
      accept(master_fd, (struct sockaddr *)&newplayer, &newplayer_addr_len);
  if (listen_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    exit(-1);
  }
  struct hostent *host_addr;
  host_addr = gethostbyaddr((char *)&newplayer.sin_addr, sizeof(struct in_addr),
                            AF_INET);

  int num_users = 1;
  for (int i = 0; i < num_users; i++) {
    sprintf(str, "%d", i);
    len = send(listen_fd, str, strlen(str), 0);
  }

  // client_connection_fd = accept(master_fd, (struct sockaddr *)&socket_addr,
  // &socket_addr_len); if (client_connection_fd == -1) {
  //   cerr << "Error: cannot accept connection on socket" << endl;
  //   return -1;
  // }  //if

  int fdmax;
  fdmax = master_fd;
  fd_set read_fds;    // temp file descriptor list for select()
  FD_ZERO(&read_fds); // clear temp sets
  for (int i = 0; i < num_users; i++) {
    if (listen_fd > fdmax) {
      fdmax = listen_fd;
    }
    FD_SET(listen_fd, &read_fds);
  }

  // send starting info to the selected player
  // cout << "sending" << str << endl;
  cout << "before select" << endl;
  int len = select(fdmax + 1, &read_fds, NULL, NULL, NULL);
  cout << "after Select" << endl;
  if (len == -1) {
    perror("select");
    exit(4);
  }

  int Player_return = 0;

  for (int i = 0; i < num_users; i++) {
    if (FD_ISSET(listen_fd, &read_fds)) {
      FD_CLR(listen_fd, &read_fds);
      Player_return = i;
      break;
    }
  }
  char return_msg[40960];
  len = recv(listen_fd, return_msg, sizeof(return_msg), 0);
  cout << return_msg << endl;
  close(master_fd);
  return 0;
}
