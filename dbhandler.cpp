#include "dbhandler.h"

pqxx::result sqlTransaction(std::string sql, connection *C) {
  try {
    work W(*C);
    result res(W.exec(sql));
    W.commit();
    return res;
  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}

// // just for test
// void DBHandler::printAccount(int account_id) {
//   string sql = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID = '" +
//                to_string(account_id) + "';";
//   result res = sqlTransaction(sql, C);
//   double balance = res[0][0].as<double>();
//   cout << "account_id: " << account_id << "     balance: " << balance <<
//   endl;
// }

void DBHandler::dropTables() {
  std::string sql =
      "DROP TABLE IF EXISTS SYMBOL, POSITION, ACCOUNT, TRANSACTION, ORDER_T;";
  sqlTransaction(sql, C);
}

void DBHandler::createTables() {
  std::string sql = "CREATE TABLE SYMBOL("
                    "SYM_ID        SERIAL PRIMARY KEY,"
                    "NAME          TEXT   NOT NULL);";
  //  work W1(*C);
  sqlTransaction(sql, C);
  sql = "CREATE TABLE ACCOUNT("
        "ACCOUNT_ID  INT    PRIMARY KEY UNIQUE,"
        "BALANCE     REAL    NOT NULL);";
  //  work W2(*C);
  sqlTransaction(sql, C);

  sql = "CREATE TABLE POSITION("
        "POS_ID      SERIAL PRIMARY KEY,"
        "SYM_ID      INT    NOT NULL,"
        "ACCOUNT_ID  INT    NOT NULL,"
        "AMOUNT      INT    NOT NULL,"
        "FOREIGN KEY (SYM_ID) REFERENCES SYMBOL(SYM_ID),"
        "FOREIGN KEY (ACCOUNT_ID) REFERENCES ACCOUNT(ACCOUNT_ID));";
  //  work W3(*C);
  sqlTransaction(sql, C);

  sql = "CREATE TABLE TRANSACTION("
        "TRANS_ID     SERIAL   PRIMARY KEY,"
        "ACCOUNT_ID   INT      NOT NULL,"
        "FOREIGN KEY (ACCOUNT_ID) REFERENCES ACCOUNT(ACCOUNT_ID));";
  //  work W4(*C);
  sqlTransaction(sql, C);

  sql = "CREATE TABLE ORDER_T("
        "ORDER_ID     SERIAL PRIMARY KEY,"
        "TRANS_ID     INT    NOT NULL,"
        "SYM_ID       INT    NOT NULL,"
        "AMOUNT       INT    NOT NULL,"
        "LIMIT_T      REAL   NOT NULL,"
        "STATUS       TEXT   NOT NULL,"
        "END_AT     TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (TRANS_ID) REFERENCES TRANSACTION(TRANS_ID),"
        "FOREIGN KEY (SYM_ID) REFERENCES SYMBOL(SYM_ID));";
  //  work W5(*C);
  sqlTransaction(sql, C);
}

DBHandler::DBHandler() {
  try {
    C = new connection(
        "host=localhost dbname=EXCH_MATCH user=postgres password=passw0rd");
    if (C->is_open()) {
    } else {
      cerr << "Can't open database" << endl;
      exit(EXIT_FAILURE);
    }
  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  dropTables();
  createTables();
}

DBHandler::~DBHandler() {
  C->disconnect();
  delete (C);
}

bool DBHandler::symbolExists(std::string sym) {
  //  nontransaction N(*C);
  std::string sql =
      "SELECT * FROM SYMBOL WHERE NAME = '" + to_string(sym) + "';";
  // result R(N.exec(sql));
  result R = sqlTransaction(sql, C);
  return !R.empty();
}

// pass unit test
void DBHandler::addSymbol(std::string sym) {
  if (symbolExists(sym)) {
    return;
  }
  //  work W(*C);
  std::string sql =
      "INSERT INTO SYMBOL (NAME) VALUES ('" + to_string(sym) + "');";
  sqlTransaction(sql, C);
}

/* getSymbolID : pass unit test
 * get the symbol id given the name
 * must have external guarantee: symbol names are unique
 */
int DBHandler::getSymbolID(std::string sym_name) {
  //  nontransaction N(*C);
  std::string sql =
      "SELECT SYM_ID FROM SYMBOL WHERE NAME='" + to_string(sym_name) + "';";
  // pqxx::result R(N.exec(sql));
  result R = sqlTransaction(sql, C);
  if (R.empty()) {
    return -1;
  }
  int res = R[0][0].as<int>();
  return res;
}

/* accoutOwnSymbol
 * to be implemented
 */
bool accountOwnSymbol(connection *C, int account_id, int sym_id) {
  //  nontransaction N(*C);
  std::string sql = "SELECT * FROM POSITION WHERE ACCOUNT_ID='" +
                    to_string(account_id) + "' AND SYM_ID='" +
                    to_string(sym_id) + "';";
  // result R(N.exec(sql));
  result R = sqlTransaction(sql, C);
  if (!R.empty())
    return true;
  return false;
}

int DBHandler::getAccountID(int trans_id) {
  //  nontransaction N(*C);
  std::string sql = "SELECT ACCOUNT_ID FROM TRANSACTION WHERE TRANS_ID = '" +
                    to_string(trans_id) + "';";
  // result R(N.exec(sql));
  result R = sqlTransaction(sql, C);
  int res = -1;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    res = c[0].as<int>();
  }
  return res;
}

// check if an account exists
bool DBHandler::validAccount(int account_id) {
  //  nontransaction N(*C);
  std::string sql = "SELECT * FROM ACCOUNT WHERE ACCOUNT_ID = '" +
                    to_string(account_id) + "';";
  // result R(N.exec(sql));
  result R = sqlTransaction(sql, C);
  return !R.empty();
}

// check how many items of sym_id does the account have
int DBHandler::amountOwned(int account_id, int sym_id) {
  //  nontransaction N(*C);
  std::string sql = "SELECT AMOUNT FROM POSITION WHERE ACCOUNT_ID = '" +
                    to_string(account_id) + "' AND SYM_ID = '" +
                    to_string(sym_id) + "';";
  // result R(N.exec(sql));
  result R = sqlTransaction(sql, C);
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    return c[0].as<int>();
  }
  return 0;
}

double DBHandler::newBalance(int account_id, double balance_delta) {
  //  nontransaction N(*C);
  std::string sql = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID = '" +
                    to_string(account_id) + "';";
  // result R(N.exec(sql));
  result R = sqlTransaction(sql, C);
  double origin_balance = R[0][0].as<double>();
  return origin_balance + balance_delta;
}

void DBHandler::updateAccount(int account_id, double new_balance) {
  //  work W(*C);
  std::string sql = "UPDATE ACCOUNT SET BALANCE = '" + to_string(new_balance) +
                    "' WHERE ACCOUNT_ID = '" + to_string(account_id) + "';";
  sqlTransaction(sql, C);
}

int DBHandler::addTransaction(int account_id) {
  //  work W(*C);
  std::string sql = "INSERT INTO TRANSACTION (ACCOUNT_ID) VALUES ('" +
                    to_string(account_id) + "') RETURNING TRANS_ID;";
  // W.commit();
  result R = sqlTransaction(sql, C);
  return R[0][0].as<int>();
}

// update to origianl_amount + amount
void DBHandler::updatePosition(int sym_id, int account_id, int amount) {
  std::string sql;
  bool owns = accountOwnSymbol(
      C, account_id, sym_id); // check if the account already owns this symbol
  if (!owns) {
    //   work W(*C);
    sql = "INSERT INTO POSITION (SYM_ID, ACCOUNT_ID, AMOUNT)VALUES ('" +
          to_string(sym_id) + "', '" + to_string(account_id) + "', '" +
          to_string(amount) + "');";
    sqlTransaction(sql, C);
  } else {
    //    cout << "account owns symbol" << endl;
    sql = "SELECT AMOUNT FROM POSITION WHERE ACCOUNT_ID='" +
          to_string(account_id) + "' AND SYM_ID= '" + to_string(sym_id) + "';";
    result R = sqlTransaction(sql, C);
    int original_amount = R[0][0].as<int>();
    int updated_amount = original_amount + amount;
    //    cout << "get updated amount " << endl;
    //    work W(*C);
    sql = "UPDATE POSITION SET AMOUNT='" + to_string(updated_amount) +
          "'WHERE ACCOUNT_ID='" + to_string(account_id) + "' AND SYM_ID='" +
          to_string(sym_id) + "';";
    sqlTransaction(sql, C);
    //    cout << "update position success" << endl;
  } // if owns
}

void DBHandler::executeOrder(int trans_id, int amount, double limit) {
  //  work W(*C);
  std::string sql = "UPDATE ORDER_T SET AMOUNT = '" + to_string(amount) +
                    "', LIMIT_T = '" + to_string(limit) +
                    "', STATUS = 'executed', "
                    "END_AT = CURRENT_TIMESTAMP "
                    "WHERE TRANS_ID = '" +
                    to_string(trans_id) + "' AND STATUS = 'open';";
  sqlTransaction(sql, C);
}

void DBHandler::cancelOrder(int trans_id) {
  //  work W(*C);
  std::string sql = "UPDATE ORDER_T SET STATUS = 'canceled', "
                    " END_AT = CURRENT_TIMESTAMP "
                    " WHERE TRANS_ID = '" +
                    to_string(trans_id) + "' AND STATUS = 'open';";
  sqlTransaction(sql, C);
}

/* addOrder : unit tested
 * add an order to the database
 */
void DBHandler::addOrder(int trans_id, int sym_id, int amount, double limit,
                         std::string status) {
  //  work W(*C);
  std::string sql = "INSERT INTO ORDER_T (TRANS_ID, SYM_ID, AMOUNT, LIMIT_T, "
                    "STATUS) VALUES ('" +
                    to_string(trans_id) + "', '" + to_string(sym_id) + "', '" +
                    to_string(amount) + "', '" + to_string(limit) + "', '" +
                    to_string(status) + "');";
  sqlTransaction(sql, C);
}

// ok if no orders currently in db
Match DBHandler::findMatch(int sym_id, int amount, double limit) {
  std::string sql;
  if (amount < 0) { // sell, find someone to buy
    // find the max limit
    sql = "SELECT MAX(LIMIT_T) FROM ( "
          "SELECT DISTINCT(LIMIT_T) FROM ORDER_T "
          "WHERE STATUS = 'open' AND SYM_ID = '" +
          to_string(sym_id) + "' AND AMOUNT > 0 AND LIMIT_T >= '" +
          to_string(limit) + "' ) AS suitable;";
    result res = sqlTransaction(sql, C);
    if (res[0][0].is_null()) {
      return {-1, 0, 0};
    }
    int max_limit = res[0][0].as<double>();
    sql = "SELECT TRANS_ID, AMOUNT, LIMIT_T FROM ORDER_T "
          "WHERE LIMIT_T = '" +
          to_string(max_limit) + "' ORDER BY TRANS_ID ASC LIMIT 1;";
  } else { // buy, find someone to sell
    sql = "SELECT MIN(LIMIT_T) FROM ( "
          "SELECT DISTINCT(LIMIT_T) FROM ORDER_T "
          "WHERE STATUS = 'open' AND SYM_ID = '" +
          to_string(sym_id) + "' AND AMOUNT < 0 AND LIMIT_T <= '" +
          to_string(limit) + "' )  AS suitable;";
    result res = sqlTransaction(sql, C);
    //    cout << "res size: " << res.size() << endl;
    //    cout << "res[0][0]: " << res[0][0] << endl;
    if (res[0][0].is_null()) {
      return {-1, 0, 0};
    }
    int min_limit = res[0][0].as<double>();
    sql = "SELECT TRANS_ID, AMOUNT, LIMIT_T FROM ORDER_T "
          "WHERE LIMIT_T = '" +
          to_string(min_limit) + "' ORDER BY TRANS_ID ASC LIMIT 1;";
  }
  result R = sqlTransaction(sql, C);
  if (R.empty())
    return {-1, 0, 0};
  int match_trans_id = R[0][0].as<int>();
  int match_amount = R[0][1].as<int>();
  ;
  double match_limit = R[0][2].as<double>();
  ;
  return {match_trans_id, match_amount, match_limit};
}

/* matchOrder
 * search in database for match order and do the transaction
 * recursively call itself if new order is generated
 */
void DBHandler::matchOrder(int account_id, int trans_id, std::string symbol,
                           int amount, double limit) {
  int sym_id = getSymbolID(symbol);
  Match match = findMatch(sym_id, amount, limit);
  //  cout << "match trans id: " << match.trans_id << endl;
  if (match.trans_id == -1) // no match orders
    return;
  int match_account_id = getAccountID(match.trans_id);
  double deal_price = match.limit;
  int deal_amount_abs = min(abs(amount), abs(match.amount));
  int deal_amount = amount > 0 ? deal_amount_abs : -deal_amount_abs;
  int match_deal_amount = -deal_amount;
  // double balance_delta = -deal_amount * deal_price;
  double balance_delta = amount > 0 ? 0 : -deal_amount * deal_price;
  double match_balance_delta = amount < 0 ? 0 : -match_deal_amount * deal_price;
  double new_balance = newBalance(account_id, balance_delta);
  double match_new_balance = newBalance(match_account_id, match_balance_delta);
  //  cout << "new balance after match: " << new_balance
  //       << ". new match balance: " << match_new_balance << endl;
  if (new_balance < 0 || match_new_balance < 0)
    return;
  // only updates the position of the buyer and the balance of the seller
  if (amount > 0) {
    updatePosition(sym_id, account_id, deal_amount);
    updateAccount(match_account_id, match_new_balance);
    // return price difference to buyer's account
    double buyer_returned = (limit - deal_price) * deal_amount_abs;
    double balance_after_return = newBalance(account_id, buyer_returned);
    updateAccount(account_id, balance_after_return);
  } else {
    updatePosition(sym_id, match_account_id, match_deal_amount);
    updateAccount(account_id, new_balance);
  }
  executeOrder(trans_id, deal_amount, deal_price);
  executeOrder(match.trans_id, match_deal_amount, deal_price);
  // cout << "order executed. account id: " << account_id << ". symbol: " <<
  // symbol
  //      << ". amount: " << amount << ". limit: " << limit << endl;
  // cout << "current balances: " << endl;
  // printAccount(account_id);
  // printAccount(match_account_id);
  // for new order generated
  int new_trans_id = -1;
  int new_amount = 0;
  double new_limit = 0;
  // match order has sth left
  if (abs(amount) == deal_amount_abs) {
    new_trans_id = match.trans_id;
    int new_amount_abs = abs(match.amount) - deal_amount_abs;
    new_amount = match.amount > 0 ? new_amount_abs : -new_amount_abs;
    new_limit = match.limit;
  } else { // this order has sth left
    new_trans_id = trans_id;
    int new_amount_abs = abs(amount) - deal_amount_abs;
    new_amount = amount > 0 ? new_amount_abs : -new_amount_abs;
    new_limit = limit;
  }
  int new_account_id = getAccountID(new_trans_id);
  if (new_amount != 0) {
    addOrder(new_trans_id, sym_id, new_amount, new_limit, "open");
    matchOrder(new_account_id, new_trans_id, symbol, new_amount, new_limit);
  }
}

void DBHandler::appendOpenOrder(TiXmlElement *element, int trans_id) {
  //  nontransaction N(*C);
  std::string sql = "SELECT AMOUNT FROM "
                    "ORDER_T WHERE STATUS = 'open' AND TRANS_ID = '" +
                    to_string(trans_id) + "';";
  // pqxx::result res(N.exec(sql));
  result res = sqlTransaction(sql, C);
  if (!res.empty()) {
    int shares = res[0][0].as<int>();
    //   cout << "open order: trans id: " << trans_id << "    amount: " <<
    //   shares
    //    << endl;
    TiXmlElement *open_order = responser.orderOpen(shares);
    element->LinkEndChild(open_order);
  }
}
void DBHandler::appendExecutedOrders(TiXmlElement *element, int trans_id) {
  std::string sql =
      "SELECT AMOUNT, LIMIT_T, TO_CHAR(END_AT, 'MON-DD-YYYY HH12:MIPM') FROM "
      "ORDER_T WHERE STATUS = 'executed' AND TRANS_ID = '" +
      to_string(trans_id) + "';";
  result res = sqlTransaction(sql, C);
  for (result::const_iterator c = res.begin(); c != res.end(); ++c) {
    int shares = c[0].as<int>();
    double price = c[1].as<double>();
    std::string executed_time = c[2].as<std::string>();
    //    cout << "executed order: trans id: " << trans_id << "    amount: " <<
    //    shares
    //    << "    price: " << price << "    time: " << executed_time << endl;
    TiXmlElement *executed_order =
        responser.orderExecuted(shares, price, executed_time);
    element->LinkEndChild(executed_order);
  }
}

void DBHandler::appendCanceledOrder(TiXmlElement *element, int trans_id) {
  std::string sql =
      "SELECT AMOUNT, TO_CHAR(END_AT, 'MON-DD-YYYY HH12:MIPM') FROM "
      "ORDER_T WHERE STATUS = 'canceled' AND TRANS_ID = '" +
      to_string(trans_id) + "';";
  result res = sqlTransaction(sql, C);
  if (!res.empty()) {
    int shares = res[0][0].as<int>();
    std::string canceled_time = res[0][1].as<std::string>();
    //    cout << "executed order: trans id: " << trans_id << "    amount: " <<
    //    shares
    //    << "    time: " << canceled_time << endl;

    TiXmlElement *canceled_order =
        responser.orderCanceled(shares, canceled_time);
    element->LinkEndChild(canceled_order);
  }
}

TiXmlElement *DBHandler::createAccount(int account_id, double balance) {
  if (validAccount(account_id))
    return responser.createAccError(account_id, "account already exists");
  if (balance < 0)
    return responser.createAccError(account_id, "balance is negative");
  //  work W(*C);
  std::string sql = "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES ('" +
                    to_string(account_id) + "', '" + to_string(balance) + "');";
  sqlTransaction(sql, C);
  //  cout << "account created" << endl;
  return responser.accountCreated(account_id);
}

TiXmlElement *DBHandler::createSymbol(std::string symbol, int account_id,
                                      int amount) {
  /*
  cout << "start create symbol" << endl;
  cout << "symbol: " << symbol << endl;
  cout << "account id: " << account_id << endl;
  cout << "amount: " << amount << endl;
  */
  if (!validAccount(account_id)) {
    return responser.createSymError(symbol, account_id,
                                    "account does not exist");
  }
  if (amount < 0) {
    return responser.createSymError(symbol, account_id, "negative num created");
  }
  if (getSymbolID(symbol) == -1) {
    //    cout << "symbol does not exist" << endl;
    //    return responser.createSymError(symbol, account_id, "symbol does not
    //    exist");

    addSymbol(symbol);
  }
  int sym_id = getSymbolID(symbol);
  updatePosition(sym_id, account_id, amount);
  return responser.symbolCreated(symbol, account_id);
}

// handle a new incoming order
TiXmlElement *DBHandler::placeOrder(int account_id, std::string symbol,
                                    int amount, double limit) {
  //  cout << "order comes in. account id: " << account_id << ". symbol: " <<
  //  symbol
  //       << ". amount: " << amount << ". limit: " << limit << endl;
  int sym_id = getSymbolID(symbol);
  double new_balance = newBalance(account_id, -amount * limit);
  if (!validAccount(account_id))
    return responser.transError(symbol, amount, limit,
                                "account id does not exist");
  if (getSymbolID(symbol) == -1)
    return responser.transError(symbol, amount, limit, "symbol does not exist");
  if (amount == 0)
    return responser.transError(symbol, amount, limit, "deal amount is 0");
  // if buy and not enough balance
  if (new_balance < 0)
    return responser.transError(symbol, amount, limit,
                                "insufficient funds in buyer account");
  // if sell and not enough items
  if (amount < 0 && amountOwned(account_id, sym_id) < abs(amount))
    return responser.transError(symbol, amount, limit,
                                "not enough items in seller account.");
  // add order to database
  int trans_id = addTransaction(account_id);
  addOrder(trans_id, sym_id, amount, limit, "open");
  // change the position if this is seller
  if (amount < 0)
    updatePosition(sym_id, account_id, amount);
  // change the balance if this is buyer
  else
    updateAccount(account_id, new_balance);
  matchOrder(account_id, trans_id, symbol, amount, limit);
  return responser.transOpened(symbol, amount, limit, trans_id);
}

TiXmlElement *DBHandler::cancelTransaction(int trans_id) {
  TiXmlElement *canceled_element = new TiXmlElement("canceled");
  canceled_element->SetAttribute("id", trans_id);
  int account_id = getAccountID(trans_id);
  if (account_id == -1)
    return responser.cancelError(trans_id);
  //  nontransaction N(*C);
  string sql = "SELECT SYM_ID, AMOUNT, LIMIT_T FROM ORDER_T"
               " WHERE TRANS_ID = '" +
               to_string(trans_id) + "' AND STATUS = 'open';";
  // pqxx::result R(N.exec(sql));
  pqxx::result R = sqlTransaction(sql, C);
  //  cout << "canceled orders #: " << R.size() << endl;
  if (!R.empty()) {
    int sym_id = R[0][0].as<int>();
    int amount = R[0][1].as<int>();
    int limit = R[0][2].as<double>();
    //    cout << "sym id: " << sym_id << endl;
    // cout << "amount: " << amount << endl;
    // cout << "limit: " << limit << endl;
    // update position if the cancelled order is sell
    if (amount < 0)
      updatePosition(sym_id, account_id, -amount);
    else { // update balance if buy
      int new_balance = newBalance(account_id, amount * limit);
      //      cout << "new balance after cancel: " << new_balance << endl;
      updateAccount(account_id, new_balance);
    }
    cancelOrder(trans_id);
  }
  // add executed records to result
  appendCanceledOrder(canceled_element, trans_id);
  appendExecutedOrders(canceled_element, trans_id);
  //  cout << "balance after cancel: " << endl;
  //  printAccount(account_id);
  return canceled_element;
}

TiXmlElement *DBHandler::queryTransaction(int trans_id) {
  if (getAccountID(trans_id) == -1)
    return responser.queryError(trans_id);
  TiXmlElement *status_element = new TiXmlElement("status");
  status_element->SetAttribute("id", trans_id);
  appendOpenOrder(status_element, trans_id);
  appendExecutedOrders(status_element, trans_id);
  appendCanceledOrder(status_element, trans_id);
  return status_element;
}
