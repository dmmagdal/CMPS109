// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <fstream>
#include <typeinfo>

#include <libgen.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

const string cin_name = "-";

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void do_ops (istream& infile, const string& filename){
   // run through each file (and perform operations)
   str_str_map test;

   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex key_regex {R"(^\s*(.*?)\s*$)"};
   regex key_equals_regex {R"(^\s*(.*?)\s*=\s*$)"};
   regex equals_regex {R"(\s*[=]\s*$)"};
   regex equals_value_regex {R"(\s*=\s*(.*?)\s*$)"};

   int linenum = 1;

   for (;;){
      string line;
      getline(infile, line);
      if (infile.eof()) break;
      cout << "-" << filename <<":" << linenum << ": "<< line << endl;
      smatch result;
      if (regex_search (line, result, comment_regex)){
         // is a comment, do nothing
         cout << line << endl;;
      }
      else if (regex_search (line, result, key_value_regex)){
         // if the key is found on the map, the value is replaced by
         // the new on. Otherwise, the new key value pair is inserted. 
         // The key value pair is printed as well.
         str_str_pair pair (result[1], result[2]);
         if (test.find(result[1]) == test.end()) test.insert(pair);
         cout << result[1] << " = " << result[2] << endl;
      }
      else if (regex_search (line, result, key_regex)){
         // find key and print key value pair if found. Print Key: key
         // not found if key could not be found
         if (test.find(result[1]) != test.end()) cout << 
            result[1] << " = " << result[2] << endl;
         else cout << result[1] << ": key not found" << endl;
      }
      else if (regex_search (line, result, key_equals_regex)){
         // if there is a whitespace after the equals, then delete the 
         // key
         if (test.find(result[1]) == test.end()) continue;
         else test.erase(test.find(result[1]));
      }
      else if (regex_search (line, result, equals_regex)){
         // print all key value pairs in lexicographic order
         for (auto i:test) cout << i.first << 
            " = " << i.second << endl;
      }
      else if (regex_search (line, result, equals_value_regex)){
         // all key value pairs with the given value are printed in 
         // lexicographic order
         for (auto i:test){
            if (i.second == result[2]) cout << i.first << 
               " = " << i.second << endl;
         }
      }
      ++linenum;
   }
   return;
}

void do_ops2 (istream& infile, const string& filename){
   // run through each file (and perform operations)
   str_str_map test;

   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex key_regex {R"(^\s*(.*?)\s*$)"};
   regex key_equals_regex {R"(^\s*(.*?)\s*=\s*$)"};
   regex equals_regex {R"(\s*[=]\s*$)"};
   regex equals_value_regex {R"(\s*=\s*(.*?)\s*$)"};

   int linenum = 1;

   for (;;){
      string line;
      getline(infile, line);
      cout << "-" << filename <<":" << linenum << ": "<< line << endl;
      smatch result;
      if (regex_search (line, result, comment_regex)){
         // is a comment, do nothing
         cout << line << endl;;
      }
      else if (regex_search (line, result, key_value_regex)){
         // if the key is found on the map, the value is replaced by
         // the new on. Otherwise, the new key value pair is inserted. 
         // The key value pair is printed as well.
         str_str_pair pair (result[1], result[2]);
         if (test.find(result[1]) == test.end()) test.insert(pair);
         cout << result[1] << " = " << result[2] << endl;
      }
      else if (regex_search (line, result, key_regex)){
         // find key and print key value pair if found. Print Key: key
         // not found if key could not be found
         if (test.find(result[1]) != test.end()) cout << 
            result[1] << " = " << result[2] << endl;
         else cout << result[1] << ": key not found" << endl;
      }
      else if (regex_search (line, result, key_equals_regex)){
         // if there is a whitespace after the equals, then delete the 
         // key
         if (test.find(result[1]) == test.end()) continue;
         else test.erase(test.find(result[1]));
      }
      else if (regex_search (line, result, equals_regex)){
         // print all key value pairs in lexicographic order
         for (auto i:test) cout << i.first << 
            " = " << i.second << endl;
      }
      else if (regex_search (line, result, equals_value_regex)){
         // all key value pairs with the given value are printed in 
         // lexicographic order
         for (auto i:test){
            if (i.second == result[2]) cout << i.first << 
               " = " << i.second << endl;
         }
      }
      ++linenum;
   }
   return;
}

int main (int argc, char** argv) {
   int status = 0;
   ifstream infile;

   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   for (int i = 1; i != argc; ++i){
      if (argc == 1) 
         do_ops2(cin, to_string(""));
      else {
         infile.open(argv[i]);
         if (infile.fail()) {
            status = 1;
            cerr << "program ecountered an error" << endl;
         }
         else {
            do_ops (infile, to_string(argv[i]));
            infile.close();
         }
      }
   }

   cout << "Status: " << status << endl;
   return EXIT_SUCCESS;
}

