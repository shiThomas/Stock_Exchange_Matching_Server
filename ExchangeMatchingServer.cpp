#include "listener.h"
#include "server.h"
//#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <mutex>
#include <thread>

#include "ThreadPool.h"
std::mutex mtx;

void server_func(std::pair<int, DBHandler *> * args) {
  //  mtx.lock();

  int newfd = args->first;
  DBHandler * handler = args->second;
  // cout << "enter thread func" << endl;
  Server server(newfd, handler);
  server.handle();
  close(newfd);
  //mtx.unlock();
}

int main(int argc, char ** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ExchangeMatchingServer <port>\n";
    exit(EXIT_FAILURE);
  }
  Listener listener(argv[1]);
  DBHandler * handler = new DBHandler();
  //  std::cout << listener.listen_fd << std::endl;
  while (1) {
    int newfd = listener.acceptRequest();
    std::pair<int, DBHandler *> * args = new std::pair<int, DBHandler *>(newfd, handler);
    //  cout << "newfd is: " << newfd << endl;
    ThreadPool pool(10);

    pool.enqueue(server_func, args);
    // std::thread t = std::thread(server_func, args);
    // t.detach();
  }
}
