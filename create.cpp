#include "create.h"

void Belong::operate(DBHandler *handler, std::string sym, TiXmlElement *root) {
  TiXmlElement *element = handler->createSymbol(sym, account_id, num);
  root->LinkEndChild(element);
}

void Account::operate(DBHandler *handler, TiXmlElement *root) {
  TiXmlElement *element = handler->createAccount(account_id, balance);
  root->LinkEndChild(element);
  // cout << "account linked to root" << endl;
}

void Symbol::operate(DBHandler *handler, TiXmlElement *root) {
  for (size_t i = 0; i < belongs.size(); i++) {
    belongs[i]->operate(handler, sym, root);
  }
}

TiXmlElement *Create::process(DBHandler *handler) {
  TiXmlElement *root_element = new TiXmlElement("create");
  for (size_t i = 0; i < operations.size(); i++) {
    operations[i]->operate(handler, root_element);
    /*
    if (string(element->Value()) == "to_remove") {
      root_element->LinkEndChild(element->FirstChildElement());
    } else {
    */
    // root_element->LinkEndChild(element);
    //}
  }
  return root_element;
}

Symbol::Symbol(std::string s) : sym(s), belongs(vector<Belong *>()) {}

Symbol::Symbol() : sym(string("")), belongs(vector<Belong *>()) {}

Symbol::~Symbol() {}

Create::Create() : operations(vector<Base1 *>()) {}

Create::~Create() {}

Belong::Belong() : account_id(-1), num(0) {}

Belong::~Belong() {}
