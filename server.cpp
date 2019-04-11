#include "server.h"

Server::Server(int newfd, DBHandler *dbhandler)
    : fd(newfd), parser(XmlParser()), handler(dbhandler) {
  //  cout << "create server with fd: " << fd << endl;
}

/*
TiXmlDocument Server::receiveXml() {
  cout << "start receiving" << endl;
  char buf[RSSIZE] = "";
  int len = recv(fd, buf, sizeof(buf), 0);

  if (len < 0) {
    cerr << "recv fails." << endl;
  }
  cout << string(buf) << endl;
  TiXmlDocument doc;
  doc.Parse(buf);
  return doc;
}
*/

TiXmlDocument Server::receiveXml() {
  // set recv timeout
  timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv))
    throw std::string("setsockopt");
  std::vector<char> msg;
  size_t index = 0;
  int nbytes;
  while (1) {
    if (msg.size() < index + RSSIZE)
      msg.resize(index + RSSIZE);
    nbytes = recv(fd, &msg.data()[index], RSSIZE - 1, 0);
    if (nbytes == -1 && msg.empty()) {
      throw std::string("recv failed");
      break;
    } else if (nbytes <= 0) {
      break;
    } else {
      index += nbytes;
    }
  }
  msg.resize(index);
  string s(msg.begin(), msg.begin() + msg.size());
  //  cout << s << endl;
  TiXmlDocument doc;
  doc.Parse(s.c_str());
  return doc;
}

void Server::sendResponse(std::string response) {
  char buf[RSSIZE];
  std::string version_msg =
      "<?xml version=\" 1.0 \" encoding=\" UTF - 8 \"?>\r\n";
  response = version_msg + response;
  strcpy(buf, response.c_str());
  // add the version line here

  if (send(fd, buf, sizeof buf, 0) < 0) {
    cerr << "send fails." << endl;
  }
}

// void test_print(Create Crt) {
//   cout << "Operation array size " << Crt.operations.size() << endl;
// }
// void trans_test_print(Transaction Tran) {
//   cout << "Operation array size " << Tran.operations.size() << endl;
// }

void Server::handle() {
  TiXmlDocument doc = receiveXml();
  TiXmlDocument res_doc;
  TiXmlElement *response;
  // cout << "TiXmlDocument" << endl;
  //  doc.Print();
  if (parser.First_Check(doc)) {
    Create create = parser.ReadAccount(doc);
    //    test_print(create);
    response = create.process(handler);
  } else {
    Transaction trans = parser.ReadTransaction(doc);

    // cout << "trans account id" << trans.account_id << endl;
    // cout << "operations size: " << trans.operations.size() << endl;
    // cout << "oeprantion 1: " << endl;
    // cout << "sym: " << ((Order *)trans.operations[0])->sym << endl;
    // cout << "amount: " << ((Order *)trans.operations[0])->amount << endl;
    // cout << "limit: " << ((Order *)trans.operations[0])->limit << endl;

    response = trans.process(handler);
  }

  res_doc.LinkEndChild(response);
  TiXmlPrinter printer;
  res_doc.Accept(&printer);
  std::string res = printer.CStr();
  //  cout << "response: " << endl;
  //  cout << res << endl;
  //  parser.destructXml(response);
  sendResponse(res);
}

Server::Server() {
  signal(SIGPIPE, SIG_IGN);
  fd = -1;
}

Server::~Server() {
  //  if (fd != -1)
  //  close(fd);
}
