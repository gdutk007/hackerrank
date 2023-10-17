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
   
   map<string,vector<order>> buyBook;
   map<string,vector<order>> sellBook;
   public:

   bool searchForId(uint64_t id){
      for(auto& iter : buyBook){
         for(auto& ord : iter.second){
            if(id == ord.id){
               return true;
            }
         }
      }
      
      for(auto& iter: sellBook){
         for(auto& ord: iter.second){
            if(id == ord.id){
               return true;
            }
         }
      }

      return false;
   }

   bool searchForIdAndOrder(uint64_t id, order& retOrder){
      for(auto& iter : buyBook){
         for(auto& ord : iter.second){
            if(id == ord.id){
               retOrder = ord;
               return true;
            }
         }
      }
      
      for(auto& iter: sellBook){
         for(auto& ord: iter.second){
            if(id == ord.id){
               retOrder = ord;
               return true;
            }
         }
      }

      return false;
   }

   order createNewOrder(vector<string> tokens){
      order order;
      order.action = tokens[0][0];
      order.id = stoul(tokens[1]);
      order.timestamp = stoul(tokens[2]);
      order.symbol = tokens[3];
      order.type = tokens[4][0];
      order.side = tokens[5][0];
      order.price = atof(tokens[6].c_str());
      order.quantity = stoul(tokens[7]);
      return order;
   }

   void readNewOrder(vector<string> tokens){

      if(tokens.size() != 8){
         cout << tokens[1] << " - Reject - 303 - Invalid order details\n";
      }

      order order = createNewOrder(tokens);

      if( searchForId(order.id) ){

         cout << tokens[1] << " - Reject - 303 - Invalid order details\n";

      } else if (order.side == 'B'){

         buyBook[order.symbol].push_back(order);

      }else if( order.side == 'S'){

         sellBook[order.symbol].push_back(order);

      }else{

         cout << tokens[1] << " - Reject - 303 - Invalid order details\n";

      }

      cout << order.id << " - Accept\n";
   }

void processAmend(vector<string> tokens){
   order order = createNewOrder(tokens);
   if( !searchForId(order.id) ){
      cout << order.id << "AmendReject - 404 - Order does not exist\n";
   }
   if(order.side == 'B'){
      for(auto& iter : buyBook){
         for(auto& ord : iter.second){
            if(   ord.timestamp <= order.timestamp &&
                  ord.symbol == order.symbol &&
                  ord.type == order.type &&
                  ord.side == order.side
               ){
                  ord.price = order.price;
                  ord.quantity = order.quantity;
                  cout << ord.id << "- AmendAccept\n";
                  return;
            }
         }
      }
   }else{
      for(auto& iter: sellBook){
         for(auto& ord: iter.second){
            if(   ord.timestamp <= order.timestamp &&
                  ord.symbol == order.symbol &&
                  ord.type == order.type &&
                  ord.side == order.side
               ){
                  ord.price = order.price;
                  ord.quantity = order.quantity;
                  cout << ord.id << "- AmendAccept\n";
                  return;
            }
         }
      }
   }
   cout << order.id << "AmendReject - 404 - Order does not exist\n";
}


 map<string,vector<order>>::iterator getMapBookByOrder(order order){
   
   auto buyIter = buyBook.begin();
   auto sellIter = sellBook.begin();

   while( buyIter != buyBook.end() ){
      for(auto& ord : buyIter->second){
         if(order.id == ord.id){
            return buyIter;
         }
      }
      ++buyIter;
   }
   while( sellIter != sellBook.end() ){
      for(auto& ord: sellIter->second){
         if(order.id == ord.id){
            return sellIter;
         }
      }
   }
   return buyBook.end();
}

void processCancel(vector<string> tokens){
   struct order order;
   order.id = stoul(tokens[1]);
   order.timestamp = stoul(tokens[2]);
   auto bookIter = getMapBookByOrder(order);
   if(bookIter == buyBook.end()){
      cout << order.id << " - CancelReject - 404 - Order does not exist\n";
      return;
   }
   for(int i = 0; i < bookIter->second.size(); ++i){
      struct order tmp = bookIter->second[i];
      if(tmp.id == order.id && tmp.timestamp <= order.timestamp){
         cout << tmp.id << " - CancelAccept" << "\n";
         bookIter->second.erase(bookIter->second.begin()+i);
         return;
      }
   }
   cout << order.id << " - CancelReject - 404 - Order does not exist\n";
}

class matchSellPrice{
   public:
   const string s{""};
   matchSellPrice() {}
   matchSellPrice(string s) : s{s} {}
   inline bool operator() (const order& lhs, const order& rhs){
      if( lhs.symbol <= rhs.symbol ){
         if(lhs.price < rhs.price){
            return true;
         }else if(lhs.price == rhs.price) {
            if(lhs.timestamp < rhs.timestamp){
               return true;
            }else{
               return false;
            }
         }else{
            return false;
         }
      }else{
         return false;
      }
   }
};

class matchBuyPrice{
   public:
   const string s{""};
   matchBuyPrice() {}
   matchBuyPrice(string s) : s{s} {}
   inline bool operator() (const order& lhs, const order& rhs){
      if( lhs.symbol <= rhs.symbol ){
         if(lhs.price > rhs.price){
            return true;
         }else if(lhs.price == rhs.price) {
            if(lhs.timestamp < rhs.timestamp){
               return true;
            }else{
               return false;
            }
         }else{
            return false;
         }
      }else{
         return false;
      }
   }
};

void processMatch(vector<string> tokens){
   // uint64_t timestamp{0};
   // string symbol{""};
   // sort(buyBook.begin(), buyBook.end(), matchBuyPrice() );
   // sort(sellBook.begin(), sellBook.end(), matchSellPrice() );
   // auto buyIter = buyBook.begin();
   // auto sellIter = sellBook.begin();
   // if(tokens.size() == 2){

   // }else{
   //    while(buyIter != buyBook.end()  && 
   //          sellIter != sellBook.end() && 
   //          buyIter->symbol != sellIter->symbol){
   //       ++buyIter;
   //       ++sellIter;
   //    }
   //    // if any iter is invalid then there is no match
   //    if(buyIter == buyBook.end() || sellIter == sellBook.end()) return;
   //    while()
   // }
}

void processQuery(vector<string> tokens){

}

};

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

   }if( tokens[0][0] == 'Q') {
      orderBook.processQuery(tokens);
   }else {

   }
}

void readInput(){
   int n{0};
   cin >> n;
   for(int i = 0; i <= n; ++i){
      string tmp{""};
      getline(cin,tmp);
      if(tmp.size()){
         readLine(tmp);
      }
   }
}


int main(){
   readInput();
   return 0;
}