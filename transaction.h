#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__
#include <string>
#include <vector>

#include "dbhandler.h"
#define ORDER 1
#define QUERY 2
#define CANCEL 3

class Base {
public:
  virtual TiXmlElement *operate(DBHandler *handler, int account_id) = 0;
};

class Order : public Base {
public:
  std::string sym;
  int amount;
  double limit;

  virtual TiXmlElement *operate(DBHandler *handler, int account_id);
  Order();
  ~Order();
};

class Query : public Base {
public:
  int query_trans_id;

  virtual TiXmlElement *operate(DBHandler *handler, int account_id);
  Query();
  ~Query();
};

class Cancel : public Base {
public:
  int cancel_trans_id;

  virtual TiXmlElement *operate(DBHandler *handler, int account_id);
  Cancel();
  ~Cancel();
};

class Transaction {
  // private:
  //  DBHandler handler;

public:
  int account_id;
  std::vector<Base *>
      operations; // subtypes polymorphism only applicable to pointers

  Transaction();
  ~Transaction();
  TiXmlElement *process(DBHandler *handler);
};

#endif
