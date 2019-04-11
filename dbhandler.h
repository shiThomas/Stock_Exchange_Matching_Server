#ifndef __DBHANDLER_H__
#define __DBHANDLER_H__
#include "responser.h"
#include "tinystr.h"
#include "tinyxml.h"
#include <algorithm>
#include <iostream>
#include <pqxx/pqxx>
#include <stdio.h>
#include <stdlib.h>
#define ORDER 1
#define QUERY 2
#define CANCEL 3
using namespace pqxx;
using namespace std;

struct _Match {
  int trans_id;
  int amount;
  double limit;
};
typedef struct _Match Match;

class DBHandler {
private:
  connection *C;
  Responser responser;
  void dropTables();
  void createTables();
  int getAccountID(int trans_id);
  bool symbolExists(std::string sym);
  int getSymbolID(std::string sym_name);
  int addTransaction(int account_id);
  void updatePosition(int sym_id, int account_id, int amount);
  int amountOwned(int account_id, int sym_id);
  double newBalance(int account_id, double balance_delta);
  void updateAccount(int account_id, double new_balance);
  void executeOrder(int trans_id, int amount, double limit);
  void cancelOrder(int trans_id);
  void addOrder(int trans_id, int sym_id, int amount, double limit,
                std::string status);
  Match findMatch(int sym_id, int amount, double limit);
  void matchOrder(int account_id, int trans_id, std::string symbol, int amount,
                  double limit);
  void appendOpenOrder(TiXmlElement *element, int trans_id);
  void appendExecutedOrders(TiXmlElement *element, int trans_id);
  void appendCanceledOrder(TiXmlElement *element, int trans_id);

public:
  DBHandler();
  ~DBHandler();
  // add the symbol name to database
  //  void printAccount(int account_id);
  bool validAccount(int account_id);
  void addSymbol(std::string sym);
  TiXmlElement *createAccount(int account_id, double balance);
  TiXmlElement *createSymbol(std::string symbol, int account_id, int amount);
  TiXmlElement *placeOrder(int account_id, std::string symbol, int amount,
                           double limit);
  TiXmlElement *cancelTransaction(int trans_id);
  TiXmlElement *queryTransaction(int trans_id);
};

#endif
