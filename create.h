#ifndef __CREATE_H__
#define __CREATE_H__
#include <stdlib.h>

#include <string>
#include <vector>

#include "dbhandler.h"

class Base1 {
public:
  virtual void operate(DBHandler *handler, TiXmlElement *root) = 0;
};

class Account : public Base1 {
public:
  int account_id;
  double balance;
  virtual void operate(DBHandler *handler, TiXmlElement *root);
};

class Belong {
public:
  int account_id;
  int num;
  void operate(DBHandler *handler, std::string sym, TiXmlElement *root);
  Belong();
  ~Belong();
};

class Symbol : public Base1 {
public:
  std::string sym;
  std::vector<Belong *> belongs;
  //  int account_id;
  //  int num;
  virtual void operate(DBHandler *handler, TiXmlElement *root);
  Symbol(std::string s);
  Symbol();
  ~Symbol();
};

class Create {
  // private:
  // DBHandler handler;

public:
  std::vector<Base1 *> operations;
  TiXmlElement *process(DBHandler *handler);
  Create();
  ~Create();
};

#endif
