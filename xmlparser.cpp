#include "xmlparser.h"

// parse position and push back to position vector
Belong *get_position(TiXmlElement *Position_Element) {
  TiXmlAttribute *Position_Acc_id = Position_Element->FirstAttribute();

  int Pos_Acc_id = stoi(Position_Acc_id->Value());

  int Share = stoi(Position_Element->FirstChild()->Value());

  Belong *bel = new Belong();
  //  cout << "POS_ACC_ID " << Pos_Acc_id << endl;
  //  cout << "Share" << Share << endl;
  bel->num = Share;
  bel->account_id = Pos_Acc_id;

  return bel;
}

/*parse information for two sets of different values;
  Account and Symbol, and push back to create object */
void get_acc(TiXmlElement *curr, Create &output) {
  std::stringstream stream;
  stream << curr->Value();
  std::string curr_child_string;
  stream >> curr_child_string; // first child ,then figure out account first or
                               // symbol first.
  if (curr_child_string[0] == 'a') {
    TiXmlAttribute *Id_Attribute = curr->FirstAttribute();    // ID
    TiXmlAttribute *balance_Attribute = Id_Attribute->Next(); // Balance

    int Account_ID = stoi(Id_Attribute->Value()); // Account ID

    //    cout << "ACC_ID " << Account_ID << endl;

    double Balance = stol(balance_Attribute->Value()); // Balance

    //    cout << "blance " << Balance << endl;
    Account *new_Acc = new Account();

    new_Acc->account_id = Account_ID;
    new_Acc->balance = Balance;
    output.operations.push_back(new_Acc);
  } else if (curr_child_string[0] == 's') {
    TiXmlAttribute *Symbol_Attribute = curr->FirstAttribute(); // SYM
    std::string Symb = Symbol_Attribute->Value();

    //  cout << "sym " << Symb << endl;
    // get account + num of share
    TiXmlElement *Position_Element = curr->FirstChildElement();
    // creatre Symbol
    Symbol *new_position = new Symbol();
    new_position->sym = Symb;
    Belong *bel = get_position(Position_Element);
    new_position->belongs.push_back(bel);

    TiXmlElement *Next_Position = Position_Element->NextSiblingElement();
    // account + num of share
    while (Next_Position) {
      Belong *bel = get_position(Next_Position);
      new_position->belongs.push_back(bel);
      Next_Position = Next_Position->NextSiblingElement();
    }
    // final push back
    output.operations.push_back(new_position);
  }
}

Create XmlParser::ReadAccount(TiXmlDocument doc) {
  std::stringstream stream;
  Create crt;
  // doc. not doc->
  //  doc.LoadFile();
  // cout << "in readAccount" << endl;
  // doc.Print();
  TiXmlElement *Root = doc.RootElement();
  const char *Root_char = Root->Value();
  stream << Root_char;
  std::string Root_string = stream.str(); // Create
  // get account element
  TiXmlElement *First_child = Root->FirstChildElement(); // account
  if (First_child) {
    get_acc(First_child, crt);
  }

  // Check if First Child exists first.
  TiXmlElement *Next_child = First_child->NextSiblingElement();
  int i = 0;
  while (Next_child) {
    cout << i++ << endl;
    get_acc(Next_child, crt);
    Next_child = Next_child->NextSiblingElement();
  }
  // cout << "next child finished" << endl;

  return crt;
}

void trans_helper(TiXmlElement *curr, Transaction &output) {
  std::stringstream stream;
  // first string
  std::string first_child_string = curr->Value();

  // order
  if (first_child_string[0] == 'o') {
    TiXmlAttribute *SYM = curr->FirstAttribute();
    std::string sym_string = SYM->Value();
    //   cout << "SYM " << sym_string << endl;
    TiXmlAttribute *AMT = SYM->Next();
    int amount = stoi(AMT->Value());
    //  cout << "amount " << amount << endl;
    TiXmlAttribute *LMT = AMT->Next();
    int limit = stoi(LMT->Value());
    //   cout << "limit " << limit << endl;
    Order *new_order = new Order();
    new_order->sym = sym_string;
    new_order->amount = amount;
    new_order->limit = limit;
    output.operations.push_back(new_order);
  }

  else if (first_child_string[0] == 'q') {
    TiXmlAttribute *Trans = curr->FirstAttribute();
    int Trans_ID = stoi(Trans->Value());
    //  cout << "Query_trans_ID " << Trans_ID << endl;
    Query *new_query = new Query();

    new_query->query_trans_id = Trans_ID;
    output.operations.push_back(new_query);
  } else if (first_child_string[0] == 'c') {
    TiXmlAttribute *Trans = curr->FirstAttribute();
    int Trans_ID = stoi(Trans->Value());
    //  cout << "Cancel_Trans_ID " << Trans_ID << endl;
    Cancel *new_cancel = new Cancel();
    new_cancel->cancel_trans_id = Trans_ID;
    output.operations.push_back(new_cancel);
  }
}

Transaction XmlParser::ReadTransaction(TiXmlDocument doc) {
  std::stringstream stream;
  Transaction trans;

  TiXmlElement *Root = doc.RootElement();

  TiXmlAttribute *Transaction_Acc_id = Root->FirstAttribute();
  int Trans_ACC_ID = stoi(Transaction_Acc_id->Value());

  trans.account_id = Trans_ACC_ID;
  //  cout << "Trans_ACC_ID" << Trans_ACC_ID << endl;
  TiXmlElement *First_child = Root->FirstChildElement();
  trans_helper(First_child, trans);
  TiXmlElement *Next_Ele = First_child->NextSiblingElement();
  int count = 0;
  while (Next_Ele) {
    //   cout << "Next " << Next_Ele->Value() << endl;
    trans_helper(Next_Ele, trans);
    Next_Ele = Next_Ele->NextSiblingElement();
    count++;
    //   cout << count << endl;
  }
  //  cout << "readTransaction success" << endl;
  return trans;
}

bool XmlParser::First_Check(TiXmlDocument doc) {
  std::stringstream stream;
  doc.LoadFile();
  TiXmlElement *Root = doc.RootElement();

  const char *Root_char = Root->Value();
  stream << Root_char;
  std::string Root_string = stream.str();
  if (Root_string[0] == 'c') {
    //   cout << "check success: create" << endl;
    return true;
  }
  if (Root_string[0] == 't') {
    //   cout << "check success: transaction" << endl;
    return false;
  }
  return Root;
}

XmlParser::XmlParser() {}
XmlParser::~XmlParser() {}

// void create_xml(string filename, string input) {
//   ofstream myfile;
//   myfile.open(filename);
//   myfile << input;
//   myfile.close();
// }

// string readxmlfile(string filename) {
//   ifstream in(filename.c_str());
//   string all, line;
//   while (getline(in, line))
//     all += line + "\r\n";
//   return all;
// }

/*
bool CreateTransaction(std::string & szfilename) {
  TiXmlDocument * trans_response = new TiXmlDocument();
  //root element
  TiXmlElement * RootElement = new TiXmlElement("results");
  trans_response->LinkEndChild(RootElement);

  if (order) {
    //create order element
    TiXmlElement * OpenElement = new TiXmlElement("opened");
    RootElement->LinkEndChild(OpenElement);
    //Attributes of open order element
    //values need to be filled in
    OpenElement->SetAttribute("sym", "SYM");
    OpenElement->SetAttribute("amount", "AMT");
    OpenElement->SetAttribute("limit", "LMT");
  }
  else if (query) {
    TiXmlElement * StatusElement = new TiXmlElement("status");
    RootElement->LinkEndChild(StatusElement);
    //attributes of status
    //trans_id needs to be replaced with a real value;
    StatusElement->SetAttribute("id", "Trans_ID");
    //while there is no order/cancel left in a transaction
    while(???){
      if (open) {
        TiXmlElement * OpElement = new TiXmlElement("open");
        StatusElement->LinkEndChild(OpElement);
        OpElement->SetAttribute("shares", "???");
      }
      //this cancel below is one query. It is different from cancel behavior
response else if (cancel) { TiXmlElement * CancelElement = new
TiXmlElement("canceled"); StatusElement->LinkEndChild(CancelElement);
        CancelElement->SetAttribute("shares", "???");
        CancelElement->SetAttribute("time", "???");
      }
      else if (executed) {
        TiXmlElement * ExecElement = new TiXmlElement("executed");
        StatusElement->LinkEndChild(ExecElement);
        ExecElement->SetAttribute("shares", "???");
        ExecElement->SetAttribute("price", "???");
        ExecElement->SetAttribute("time", "???");
      }
    }
  }
  else if (cancel) {
    TiXmlElement * CanElement = newTiXmlElement("canceled");
    RootElement->LinkEndChild(CanElement);
    CanElement->SetAttribute("Trans_ID", "???");
    if (open) {
      TiXmlElement * CanceledElement = new TiXmlElement("canceled");
      CanElement->LinkEndChild(CanceledElement);
      CanceledElement->SetAttribute("shares", "???");
      CanceledElement->SetAttribute("time", "???");
    }
    //not open
    else {
      TiXmlElement * ExecElement = new TiXmlElement("executed");
      CanElement->LinkEndChild(ExecElement);
      ExecElement->SetAttribute("shares", "???");
      ExecElement->SetAttribute("price", "???");
      ExecElement->SetAttribute("time", "???");
    }
  }
}
*/
