// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <algorithm>                                                          // for min() max()
#include <cmath>                                                              // for floor()
#include <vector>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that): ubig_value (that) {
   //DEBUGF ('~', this << " -> " << ubig_value)
   ubigint (to_string(that));
}

ubigint::ubigint (const string& that): ubig_value(0) {
   //DEBUGF ('~', "that = \"" << that << "\"");
   /*
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      // uvalue = uvalue * 10 + digit - '0'; 
      ubig_value.push_back(digit);                                            // add char to the vector
   }*/
   for (int i = that.length()-1; i >= 0; --i){
      this->ubig_value.push_back(that[i]-'0');
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   //return ubigint (uvalue + that.uvalue);
   ubigint sum = ubigint();                                                   // create new ubigint object to hold the sum   
   int c = 0;                                                                 // carry over variable
   int thisSize = ubig_value.size();
   int thatSize = that.ubig_value.size();
   for (int i = 0; i < max(thisSize, thatSize); ++i){                         // iterate through ubigint 
      if (i < min(thisSize, thatSize)){                                       // if the iterator i is less that the min of the two ubigints
         int s = ubig_value[i] + that.ubig_value[i] + c;                      // add the indices of both vectors and the carry over to an int
         if (s > 9){                                                          // if the sum is greater than 9
            sum.ubig_value.push_back(s % 10);                                 // push the sum mod 10 to the sum ubigint
            c = 1;                                                            // set the carry over to 1 (carry over will never be greater than 1)
         }
         else {                                                               // otherwise (sum is les than or equal to 9)
            sum.ubig_value.push_back(s);                                      // push the sum to the sum ubigint
            c = 0;                                                            // set the carry over to 0
         }
      }
      else {                                                                  // otherwise (the iterator i is at or greater than the min of the two ubigints. This will never be executed if the two ubigints are the same size)
         if (c != 0 and ubig_value.size() > that.ubig_value.size()){          // if the carry over is not zero and this ubigint is bigger than that ubigint
            int s = c + ubig_value[i];                                        // another check for if sum is greater than 9 (repeat of process above)
            if (s > 9){
               sum.ubig_value.push_back(s % 10);                              
               c = 1;                                                         
            }
            else {
               sum.ubig_value.push_back(s);
               c = 0;
            }
            
         }
         else if (c == 0 and ubig_value.size() > that.ubig_value.size()){     // else if the carry over is zero and this ubigint is bigger than that ubigint
            sum.ubig_value.push_back(ubig_value[i]);                          // push the sum of this ubigint at index onto the sum ubigint
         }
         else if (c != 0 and ubig_value.size() < that.ubig_value.size()){     // else if the carry over is not zero and this ubigint is less than that ubigint
            int s = c + that.ubig_value[i];                                   // another check for if sum is greater than 9 (repeat of process above)
            if (s > 9){
               sum.ubig_value.push_back(s % 10);                              
               c = 1;                                                         
            }
            else {
               sum.ubig_value.push_back(s);
               c = 0;
            }
         }
         else if (c == 0 and ubig_value.size() < that.ubig_value.size()){     // else if the carry over is zero and this ubigint is less than that ubigint
            sum.ubig_value.push_back(that.ubig_value[i]);                     // push the sum of that ubigint at index onto the sum ubigint
         }
      }
   }
   if (c == 1){                                                               // final check: if there is still a nonzero carry over, push it onto the sum vector
      sum.ubig_value.push_back(c);
   }
   return sum;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   //return ubigint (uvalue - that.uvalue);
   ubigint diff = ubigint();                                                  // create new ubigint object to hold the difference  
   if (*this == that){                                                        // if the two vectors are equal
      diff.ubig_value.push_back(0);                                           // push zero onto the diff vector
   }
   else {                                                                     // otherwise, they are not equivalent vectors so conitnue on
      int thisSize = ubig_value.size();
      int thatSize = that.ubig_value.size();
      int c = 0;                                                              // carry over vairable
      for (int i = 0; i < thisSize; ++i){                                     // iterate through this ubigint 
         int op1digit = ubig_value[i];                                        // variables to store the operands respectively and the difference digits
         int op2digit;
         int difference;
         if (i >= thatSize){                                                  // if the index is out of the scope of the second operand vector
            op2digit = 0;                                                     // set the second operand digit to zero
         }
         else {                                                               // otherwise (the index is within the scope of both vectores)
            op2digit = that.ubig_value[i];                                    // set the second operand digit to the respective value in that vector
         }
         if (c != 0){                                                         // if there had been a previous carry over
            if (op1digit - c >= 0){                                          // if taking this carry from the first operand
               op1digit -= c;                                                 // subtract the carry over from the first operand
            }
            else {
               op2digit += c;                                                 // add the carry over to the other operand
            }
            c = 0;                                                            // reset the carry over
         }
         if (op1digit - op2digit < 0){                                        // if subtracting the operand digits will be negative
            difference = 10 + op1digit - op2digit;                            // set the difference to 10 + the difference of the operands
            c += 1;                                                           // add to the carry over since it's needed
         }
         else {                                                               // otherwise (we need no carry over)
            difference = op1digit - op2digit;                                 // subtract the operands accordingly and set the difference to the difference digit
         }
         diff.ubig_value.push_back(difference);                               // push the difference to the diff vector
      }
      diff.trim();                                                            // remove leading zeroes     
   }
   return diff;
}

ubigint ubigint::operator* (const ubigint& that) const {
   //return ubigint (uvalue * that.uvalue);
   int thisSize = ubig_value.size();
   int thatSize = that.ubig_value.size();
   ubigint prod = ubigint();
   prod.ubig_value.resize(thisSize+thatSize, 0);
   for (int i = 0; i < thisSize; ++i){
      int c = 0;
      for (int j = 0; j < thatSize; ++j){
         int d = prod.ubig_value[i+j] + (ubig_value[i]*that.ubig_value[j]) + c;
         prod.ubig_value[i+j] = d%10;
         c = floor(d/10);
      }
      prod.ubig_value[i+thatSize] = c;
   }
   prod.trim();
   return prod;
}

void ubigint::multiply_by_2() {
   //uvalue *= 2;
   int thisSize = ubig_value.size();
   int c = 0;                                                                 // carry over variable
   for (int i = 0; i < thisSize; ++i){                                        // iterate through the vector and multiply each element by 2 and store it back
      // ubig_value[i] *= 2;
      int temp = ubig_value[i];
      int p = (temp * 2) + c;
      if (p > 9){
         ubig_value[i] = p % 10;
         c = 1;
      }
      else {
         ubig_value[i] = p;
         c = 0;
      }
   }
   if (c != 0){
      ubig_value.push_back(c);
   }
}

void ubigint::divide_by_2() {
   // uvalue /= 2;
   // if odd multiply by five and then divide by 10 (take off last decimal)
   int thisSize = ubig_value.size();
   int c = 0;
   for (int i = 0; i < thisSize; ++i){                                        // iterate through the vector and multiply each element by 5 and store it back
      int temp = ubig_value[i];
      int five = (temp * 5) + c;
      if (five > 9){
         ubig_value[i] = five % 10;
         c = five / 10;
      }
      else{
         ubig_value[i] = five;
         c = 0;
      }
   }
   if (c != 0){
      ubig_value.push_back(c);
   }
   ubig_value.erase(ubig_value.begin());                                      // remove the last decimal (easy divide by 10)
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, ubigint divisor) {
   // Note: divisor is modified so pass by value (copy).
   ubigint zero {"0"};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {"1"};
   ubigint quotient {"0"};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor == remainder or divisor < remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   //return ubig_value == that.ubig_value;
   bool isequal = true;
   int thisSize = ubig_value.size();
   int thatSize = that.ubig_value.size();
   if (thisSize == thatSize){
      for (int i = 0; i < thisSize; i++){
         if (ubig_value[i] != that.ubig_value[i]){
            return false;
         }
      }
   }
   else {
      isequal = false;
   }
   return isequal;
}

bool ubigint::operator< (const ubigint& that) const {
   //return ubig_value < that.ubig_value;
   bool islessthan = false;
   int thisSize = ubig_value.size();
   int thatSize = that.ubig_value.size();
   if (ubig_value == that.ubig_value){
      return false;
   }
   else if (thisSize < thatSize){
      return true;
   }
   else if (thisSize > thatSize){
      return false;
   }
   else {
      for (int i = that.ubig_value.size()-1; i >= 0; --i){
         if (ubig_value[i] > that.ubig_value[i]){
            return false;
         }
         else if (ubig_value[i] < that.ubig_value[i]){
            return true;
         }
      }
   }
   return islessthan;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   //return out << "ubigint(" << that.ubig_value << ")";
   string ret = "";
   // for (auto i = that.ubig_value.crbegin(); i != that.ubig_value.crend(); --i){
   for (int i = that.ubig_value.size()-1; i >= 0; --i){
      ret += to_string(that.ubig_value[i]);
      //ret += *i;
   }
   return out << ret;
}

void ubigint::trim () {
   while (ubig_value[ubig_value.size()-1] == 0){
      ubig_value.erase(--ubig_value.cend());
   }
}
