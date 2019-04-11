#ifndef __XMLPARSER_H__
#define __XMLPARSER_H__
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "create.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "transaction.h"
class XmlParser {
public:
  XmlParser();
  ~XmlParser();
  Create ReadAccount(
      TiXmlDocument doc); // input may not be this type or (string filepath??)
  Transaction ReadTransaction(TiXmlDocument doc);
  bool First_Check(TiXmlDocument doc);
  // choose to parse to Account or Transaction
};
#endif
