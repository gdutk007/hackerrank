#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <algorithm>
#include <stdexcept>
#include <set>
#include <list>
#include <map>
#include <queue>
using namespace std;

struct order{
   char action{'\0'};
   uint64_t id{0};
   uint64_t timestamp{0};
   string symbol{""};
   char type{'\0'};
   char side{'\0'};
   float price{0.0};
   uint64_t quantity{0};
};

class matchingBook{

   vector<order> orders;
   public:

   bool searchForId(uint64_t id, int& index){
      for(int i = 0; i < orders.size(); ++i){
         if(orders[i].id == id){
            index = i;
            return true;
         }
      }
      return false;
   }

   bool isFloat(string token){
      for(auto& c : token){
         if(c == '.') return true;
      }
      return false;
   }

   bool createNewOrder(vector<string> tokens, struct order& order){
      try{
         order.action = tokens[0][0];
         
         if(isFloat(tokens[1])) return false;
         order.id = stoul(tokens[1]);
         
         if(isFloat(tokens[2])) return false;
         order.timestamp = stoul(tokens[2]);
         order.symbol = tokens[3];
         order.type = tokens[4][0];
         order.side = tokens[5][0];
         order.price = atof(tokens[6].c_str());
         
         if(isFloat(tokens[7])) return false;
         order.quantity = stol(tokens[7]);

      }catch(const exception& e){
         return false;
      }
      return true;
   }


void readNewOrder(vector<string> tokens){
      if(tokens.size() != 8){
         cout << tokens[1] << " - Reject - 303 - Invalid order details\n";
      }
      struct order order;
      if(!createNewOrder(tokens, order)){
         cout << tokens[1] << " - Reject - 303 - Invalid order details\n";
         return;
      }
      int index = -1;
      if(!searchForId(order.id, index)){
         orders.push_back(order);
         cout << order.id << " - Accept\n";
      }else{
         cout << tokens[1] << " - Reject - 303 - Invalid order details\n";
      }
}

void processAmend(vector<string> tokens){
   struct order order;
   bool found = createNewOrder(tokens, order);
   // if(found) cout << "found it.\n";
   // else cout << "not found\n";
   int index = -1;
   //cout  << "id:  " <<  order.id << "index:" << index << "\n";
   if( searchForId(order.id,index) ){
      struct order ord = orders[index];
      if(   ord.timestamp < order.timestamp &&
            ord.symbol == order.symbol &&
            ord.type == order.type &&
            ord.side == order.side ){
               ord.price = order.price;
               ord.quantity = order.quantity;
               cout << ord.id << "- AmendAccept\n";
               return;
      }else{
         cout << order.id << " AmendReject - 101 - Order does not exist\n";
         return;
      }
   }
   cout << order.id << " AmendReject - 404 - Order does not exist\n";
}

void processCancel(vector<string> tokens){
   struct order order;
   order.id = stoul(tokens[1]);
   order.timestamp = stoul(tokens[2]);
   int index = -1;
   if( searchForId(order.id, index) ){
      if( order.id == orders[index].id && orders[index].timestamp <= order.timestamp ){
         cout << orders[index].id << " - CancelAccept\n";
         orders.erase(orders.begin()+index);
         return;
      }
   }
   cout << order.id << " - CancelReject - 404 - Order does not exist\n";
}

class sortBuy{
   public:
   inline bool operator() (const order& lhs, const order& rhs){
      return (lhs.price < rhs.price);
   }
};

class sortSell{
   public:
   inline bool operator() (const order& lhs, const order& rhs){
      return (lhs.price > rhs.price);
   }
};

class compareTime{
   public:
   inline bool operator() (const order& lhs, const order& rhs){
      return (lhs.timestamp < rhs.timestamp);
   }
};

void printMatch(order b, order s){
   cout << b.symbol << "|" << b.id << "," << b.type << "," << b.quantity << "," << b.price << "|"
   << s.price << "," << s.quantity << "," << s.type << "," << s.id << "\n";
}

void processMatch(vector<string> tokens){
   string symbol{""};
   long timestamp = stol(tokens[1]);
   map<string, vector<order>> orderMap;
   vector<order> newOrderBook;

   if(tokens.size() == 3) 
      symbol = tokens[2];

   for(auto order : orders){
      orderMap[order.symbol].push_back(order);
   }
   if(symbol.size()){
      orderMap[symbol] = processMatches(orderMap[symbol], timestamp);
   }else{
      for(auto orderIter = orderMap.begin(); orderIter != orderMap.end(); ++orderIter ){
         orderIter->second = processMatches(orderIter->second, timestamp);
      }
   }
   // generate new orderBook
   for(auto orderIter = orderMap.begin(); orderIter != orderMap.end(); ++orderIter ){
      for(auto& o : orderIter->second){
         newOrderBook.push_back(o);
      }
   }
   // assign new orderBook
   orders = newOrderBook;
}

vector<order> processMatches(vector<order>& os, long timestamp){
   sort(os.begin(), os.end(), compareTime());
   priority_queue<order, vector<order>, sortBuy > buy;
   priority_queue<order, vector<order>, sortSell > sell;
   vector<order> newOrders;
   for(auto& o : os){
      if(timestamp >= 0 && o.timestamp > timestamp)
         break;

      if(o.side == 'B'){
         buy.push(o);
      }else{
         sell.push(o);
      }
      while(buy.size() && sell.size() && buy.top().price >= sell.top().price){
         int k = min(buy.top().quantity, sell.top().quantity);
         printMatch(buy.top(), sell.top());
         struct order tmp = buy.top();
         buy.pop();
         tmp.quantity -= k;
         if(tmp.quantity)buy.push(tmp);
         tmp = sell.top();
         sell.pop();
         tmp.quantity -= k;
         if(tmp.quantity)sell.push(tmp);
      }
   }
   while(buy.size()){
      newOrders.push_back(buy.top());
      buy.pop();
   }
   while(sell.size()){
      newOrders.push_back(sell.top());
      sell.pop();
   }
   return newOrders;
}

void query(vector<order> os, long timestamp){
   sort(os.begin(), os.end(), compareTime());
   priority_queue<order, vector<order>, sortBuy> buy;
   priority_queue<order,vector<order>, sortSell> sell;
   for(auto & o : os){
      if(timestamp > 0 && o.timestamp > timestamp)
         break;
      if( o.side == 'B' ){
         buy.push(o);
      }else{
         sell.push(o);
      }
      while(buy.size() && sell.size() && buy.top().price >= sell.top().price){
         int k = min(buy.top().quantity,sell.top().quantity);
         printMatch(buy.top(),sell.top());
         struct order tmp = buy.top();
         buy.pop();
         tmp.quantity -= k;
         if(tmp.quantity) buy.push(tmp);
         tmp = sell.top();
         sell.pop();
         tmp.quantity -= k;
         if(tmp.quantity) sell.push(tmp);
      }
   }
   while(buy.size()){
      struct order b = buy.top();
      cout << b.symbol << "|" << b.id << "," << b.type << "," << b.quantity << "," << b.price << "||\n";
      buy.pop();
   }
   while(sell.size()){
      struct order s = sell.top();
      cout << s.symbol << "||" << s.price << "," << s.quantity << "," << s.type << "," << s.id << "\n";
      sell.pop();
   }
}

void printQuery(string symbol, long timestamp){
   map<string, vector<order>> orderMap;
   for(auto& o : orders){
      orderMap[o.symbol].push_back(o);
   }
   if(symbol.size()){
      query(orderMap[symbol], timestamp);
   }else{
      for(auto iter = orderMap.begin(); iter != orderMap.end(); ++iter){
         query(iter->second, timestamp);
      }
   }
}

void processQuery(vector<string> tokens){
   int i = 1;
   int n = tokens.size();
   string symbol{""};
   long timestamp{-1};
   for(int i = 1; i < tokens.size(); ++i){
      if( isdigit(tokens[i][0]) ){
         timestamp = stol(tokens[i]);
      }else{  
         symbol = tokens[i];
      }
   }
   printQuery(symbol, timestamp);
}

};

// 4
// N,2,0000002,XYZ,L,B,104.53,100
// N,3,0000002,XYZ,L,B,104.53,100.3
// A,2,0000001,XYZ,L,B,103.53,150
// A,2,0000001,XYZ,L,S,103.53,150

// 3
// N,2,0000001,XYZ,L,B,104.53,100
// A,2,0000001,XYZ,L,B,103.53,150
// A,2,0000001,XYZ,L,S,103.53,150

// 7
// N,1,0000001,ALN,L,B,60.90,100
// N,11,0000002,XYZ,L,B,60.90,200
// N,110,0000003,XYZ,L,S,60.90,100
// N,112,0000003,XYZ,L,S,60.90,120
// N,10,0000006,ALN,L,S,60.90,100
// M,00010
// M,00010,ALN

// 6
// N,1,0000001,ALN,L,B,60.90,100
// N,11,0000002,XYZ,L,B,60.90,200
// N,110,0000003,XYZ,L,S,60.90,100
// N,112,0000003,XYZ,L,S,60.90,120
// N,10,0000006,ALN,L,S,60.90,100
// M,00010,ALN

// 14
// N,1,0000001,ALN,L,B,60.90,100
// N,13,0000002,ALN,L,B,60.90,100
// N,10,0000003,ALN,L,S,60.90,100
// N,12,0000004,ALN,L,S,60.90,100
// N,11,0000005,ALB,L,S,60.90,100
// N,14,0000006,ALB,L,S,62.90,101
// N,16,0000007,ALB,L,S,63.90,102
// N,18,0000008,ALB,L,S,64.90,103
// N,20,0000009,ALB,L,S,65.90,104
// Q,0000003
// Q,ALB
// Q,ALN,0000002
// Q,0000002,ALN
// Q

// 7
// N,1,0000001,AB,L,B,104.53,100
// N,2,0000002,AB,L,S,105.53,100
// N,3,0000003,AB,L,B,104.53,90
// M,0000004
// N,4,0000005,AB,L,S,104.43,80
// A,2,0000006,AB,L,S,104.42,100
// Q
// M,0000008
// N,5,0000009,AB,L,S,105.53,120
// X,3,0000010
// N,6,0000011,XYZ,L,B,1214.82,2568
// Q

matchingBook orderBook;

void readLine(string line){
   stringstream ss(line);
   string tmp{""};
   vector<string> tokens;
   while( getline( ss, tmp,',' ) ){
      tokens.push_back(tmp);
   }

   if( tokens[0][0] == 'N') {
      orderBook.readNewOrder(tokens);
   }else  if( tokens[0][0] == 'A') {
      orderBook.processAmend(tokens);
   }if( tokens[0][0] == 'X') {
      orderBook.processCancel(tokens);
   }else  if( tokens[0][0] == 'M') {
      orderBook.processMatch(tokens);
   }if( tokens[0][0] == 'Q') {
      orderBook.processQuery(tokens);
   }else {

   }
}

void readInput(){
   int n{0};
   cin >> n;
   vector<string> lines;
   for(int i = 0; i <= n; ++i){
      string tmp{""};
      getline(cin,tmp);
      if(tmp.size()){
         //readLine(tmp);
         lines.push_back(tmp);
      }
   }
   for(auto& line : lines){
      readLine(line);
   }
}


int main(){
   readInput();
   return 0;
}