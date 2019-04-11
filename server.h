#ifndef __SERVER_H__
#define __SERVER_H__

#include "dbhandler.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "xmlparser.h"
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define RSSIZE 65536
class Server {
private:
  int fd;
  XmlParser parser;
  DBHandler *handler;
  TiXmlDocument receiveXml();
  void sendResponse(std::string response);

public:
  Server(int newfd, DBHandler *dbhandler);
  Server();
  ~Server();
  void handle();
};

#endif
