#ifndef __RESPONSER_H__
#define __RESPONSER_H__
#include "tinystr.h"
#include "tinyxml.h"
#include <iostream>
#include <string>
class Responser {

public:
  Responser();
  ~Responser();
  // problem: when are the TiXmlElements destructed?
  TiXmlElement *createAccError(int account_id, std::string msg);
  TiXmlElement *createSymError(std::string symbol, int account_id,
                               std::string msg);
  TiXmlElement *accountCreated(int account_id);
  TiXmlElement *symbolCreated(std::string symbol, int account_id);
  TiXmlElement *orderOpen(int shares);
  TiXmlElement *cancelError(int trans_id);
  TiXmlElement *queryError(int trans_id);
  TiXmlElement *orderCanceled(int shares, std::string time);
  TiXmlElement *orderExecuted(int shares, double price, std::string time);
  TiXmlElement *transOpened(std::string symbol, int amount, double limit,
                            int trans_id);
  TiXmlElement *transError(std::string symbol, int amount, double limit,
                           std::string msg);
  // TiXmlElement *transStatus(int trans_id);
  // TiXmlElement *transCanceled(int trans_id);
};
#endif
