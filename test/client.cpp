#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
int status;
// int player_fd;
int len;
int player_id;
int port_num_master;
struct sockaddr_in master_sock;
char buffer[40960];
struct hostent *master_host_addr;
struct sockaddr_in player_in;
const char *port;
using namespace std;

int set_client(const char *host, int port_num_master) {
  master_sock.sin_family = AF_INET;
  master_sock.sin_port = htons(port_num_master);
  memcpy(&master_sock.sin_addr, master_host_addr->h_addr_list[0],
         master_host_addr->h_length);

  int player_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (player_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    exit(-1);
  } // if

  status =
      connect(player_fd, (struct sockaddr *)&master_sock, sizeof(master_sock));
  ;
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    exit(-1);
  } // if

  char plyr_id[64];
  // len = recv(player_fd, plyr_id, sizeof(plyr_id), 0);
  plyr_id[len] = '\0';
  return player_fd;
  //  player_id = atoi(plyr_id);
}

void request(int fd, string fn) {
  ifstream in(fn.c_str());
  string all, line;
  while (getline(in, line))
    all += line + "\r\n";

  // const char * message = "hi there!";
  const char *message = all.c_str();
  cout << message << endl;
  // send(socket_fd, all, all.size(), 0);
  send(fd, message, strlen(message), 0);
  // cout << "send success" << endl;
  // sleep(3);

  char receive_msg[40960];
  len = recv(fd, receive_msg, sizeof(receive_msg), 0);
  receive_msg[len] = '\0';
  cout << receive_msg << endl; // sleep(3);
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  const char *host = NULL;
  if (argc != 4) {
    cout << "Syntax:" << argv[0] << "<machine_name> <port_num><filename>"
         << endl;

    return 1;
  }
  master_host_addr = gethostbyname(argv[1]);
  if (master_host_addr == NULL) {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }

  host = argv[1];

  port = argv[2];
  port_num_master = atoi(argv[2]);
  // acquire player_id here;

  int fd = set_client(host, port_num_master);
  // server set up
  // cout << user_id << endl;
  /*

     The following lines are used to


     test transferring files



  */
  //  string fn = "trans.xml";
  char *fn_str = argv[3];
  string fn(fn_str);
  request(fd, fn);
  close(fd);
  /*
  close(fd);
  fd = set_client(host, port_num_master);
  request(fd, "trans1.xml");
  close(fd);
  */

  return 0;
}
