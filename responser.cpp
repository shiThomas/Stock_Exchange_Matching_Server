#include "responser.h"

TiXmlElement *Responser::createAccError(int account_id, std::string msg) {
  TiXmlElement *error_element = new TiXmlElement("error");
  error_element->SetAttribute("id", account_id);
  TiXmlText *text = new TiXmlText(msg.c_str());
  error_element->LinkEndChild(text);
  return error_element;
}

TiXmlElement *Responser::createSymError(std::string symbol, int account_id,
                                        std::string msg) {
  TiXmlElement *error_element = new TiXmlElement("error");
  error_element->SetAttribute("sym", symbol.c_str());
  error_element->SetAttribute("id", account_id);
  TiXmlText *text = new TiXmlText(msg.c_str());
  error_element->LinkEndChild(text);
  return error_element;
}
TiXmlElement *Responser::accountCreated(int account_id) {
  TiXmlElement *created_element = new TiXmlElement("created");
  created_element->SetAttribute("id", account_id);
  return created_element;
}
TiXmlElement *Responser::symbolCreated(std::string symbol, int account_id) {
  TiXmlElement *created_element = new TiXmlElement("created");
  created_element->SetAttribute("sym", symbol.c_str());
  created_element->SetAttribute("id", account_id);
  return created_element;
}

TiXmlElement *Responser::orderOpen(int shares) {
  TiXmlElement *open_order = new TiXmlElement("open");
  open_order->SetAttribute("shares", shares);
  return open_order;
}
TiXmlElement *Responser::orderCanceled(int shares, std::string time) {
  TiXmlElement *canceled_order = new TiXmlElement("canceled");
  canceled_order->SetAttribute("shares", shares);
  canceled_order->SetAttribute("time", time.c_str());
  return canceled_order;
}

TiXmlElement *Responser::orderExecuted(int shares, double price,
                                       std::string time) {
  TiXmlElement *executed_order = new TiXmlElement("executed");
  executed_order->SetAttribute("shares", shares);
  executed_order->SetAttribute("price", price);
  executed_order->SetAttribute("time", time.c_str());
  return executed_order;
}

TiXmlElement *Responser::transOpened(std::string symbol, int amount,
                                     double limit, int trans_id) {
  TiXmlElement *opened_element = new TiXmlElement("opened");
  opened_element->SetAttribute("sym", symbol.c_str());
  opened_element->SetAttribute("amount", amount);
  opened_element->SetAttribute("limit", limit);
  opened_element->SetAttribute("id", trans_id);
  return opened_element;
}

TiXmlElement *Responser::transError(std::string symbol, int amount,
                                    double limit, std::string msg) {
  TiXmlElement *error_element = new TiXmlElement("error");
  TiXmlText *text = new TiXmlText(msg.c_str());
  error_element->SetAttribute("sym", symbol.c_str());
  error_element->SetAttribute("amount", amount);
  error_element->SetAttribute("limit", limit);
  error_element->LinkEndChild(text);
  return error_element;
}

TiXmlElement *Responser::cancelError(int trans_id) {
  std::string msg = "trans_id " + std::to_string(trans_id) + " does not exist";
  TiXmlElement *error_element = new TiXmlElement("error");
  TiXmlText *text = new TiXmlText(msg.c_str());
  error_element->LinkEndChild(text);
  return error_element;
}

TiXmlElement *Responser::queryError(int trans_id) {
  std::string msg = "trans_id " + std::to_string(trans_id) + " does not exist";
  TiXmlElement *error_element = new TiXmlElement("error");
  TiXmlText *text = new TiXmlText(msg.c_str());
  error_element->LinkEndChild(text);
  return error_element;
}
Responser::Responser() {}
Responser::~Responser() {}
