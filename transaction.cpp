#include "transaction.h"

TiXmlElement *Order::operate(DBHandler *handler, int account_id) {
  return handler->placeOrder(account_id, sym, amount, limit);
}

// actually don't need any parameter. Just for convenience of polymorphism
TiXmlElement *Query::operate(DBHandler *handler, int account_id) {
  return handler->queryTransaction(query_trans_id);
}

// actually don't need any parameter. Just for convenience of polymorphism
TiXmlElement *Cancel::operate(DBHandler *handler, int account_id) {
  return handler->cancelTransaction(cancel_trans_id);
}

TiXmlElement *Transaction::process(DBHandler *handler) {
  if (!handler->validAccount(account_id)) {
    TiXmlElement *error_element = new TiXmlElement("results");
    std::string msg =
        "account_id " + std::to_string(account_id) + " does not exist";
    TiXmlText *text = new TiXmlText(msg.c_str());
    error_element->LinkEndChild(text);
    //  handler->printAccount(account_id);
    return error_element;
  }
  TiXmlElement *root_element = new TiXmlElement("results");
  for (size_t i = 0; i < operations.size(); i++) {
    TiXmlElement *element = operations[i]->operate(handler, account_id);
    root_element->LinkEndChild(element);
  }
  // handler->printAccount(account_id);
  return root_element;
}

Transaction::Transaction() {}

Transaction::~Transaction() {}

Order::Order() : sym(""), amount(0), limit(0) {}

Order::~Order() {}

Query::Query() : query_trans_id(-1) {}

Query::~Query() {}

Cancel::Cancel() : cancel_trans_id(-1) {}

Cancel::~Cancel() {}
