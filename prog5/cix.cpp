// $Id: cix.cpp,v 1.4 2016-05-09 16:01:56-07 - - $

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream log (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"put" , cix_command::PUT },
   {"get" , cix_command::GET },
   {"rm"  , cix_command::RM  }
};

void cix_help() {
   static const vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      log << "sent LS, server did not return LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.nbytes + 1];
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer;
   }
}

void cix_put (client_socket& server, string& nameOfFile) {
   // cout << "in cix_put" << endl; // debugging tool

   // check size of filename
   if (nameOfFile.size() > 59) log << "Error: name of file, " << 
      nameOfFile << ", is too long." << endl;

   // cout << "length of fileName " << nameOfFile.size()<< endl;

   // open file and read from file
   ifstream is (nameOfFile, ifstream::binary);
   int length = 0;
   if (is) {
      // tell length
      is.seekg(0, is.end);
      length = is.tellg();
      is.seekg(0, is.beg);

      //cout << "length of "<< nameOfFile << " is " << 
      //   length << endl;

      char * bufferstring = new char [length+1];
      is.read(bufferstring, length);
      // cout << bufferstring << endl; // test to make sure file 
      //       is being copied correctly to the buffer

      // create header and payload
      cix_header header;
      header.command = cix_command::PUT;
      header.nbytes = length;
      strcpy(header.filename, nameOfFile.c_str());

      is.close();
      
      // send header to server
      log << "sending header " << header << endl;
      send_packet (server, &header, sizeof header);
      send_packet (server, &bufferstring, header.nbytes);
      recv_packet (server, &header, sizeof header);
      log << "received header " << header << endl;

      // check if header was sent successfully
      if (header.command != cix_command::ACK) {
         log << "sent PUT, server did not return ACK" << endl;
         log << "server returned " << header << endl;
      }
      else {
         char buffer[header.nbytes + 1];
         recv_packet (server, buffer, header.nbytes);
         log << "received " << header.nbytes << " bytes" << endl;
         buffer[header.nbytes] = '\0';
      }

      // delete variables and close istream
      delete[] bufferstring;
   }
}


void cix_get (client_socket& server, string& nameOfFile) {
   cout << "in cix_get" << endl; // debugging tool

   // check size of filename
   if (nameOfFile.size() > 59) log << "Error: name of file, " << 
      nameOfFile << ", is too long." << endl;

   cix_header header;
   header.command = cix_command::GET;
   header.nbytes = 0;
   strcpy(header.filename, nameOfFile.c_str());

   // send header to server
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;

   if (header.command != cix_command::FILE) {
      log << "sent GET, server did not return FILE" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.nbytes + 1];
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';

      // write recieved file to server
      ofstream writefile (header.filename, ofstream::binary);
      writefile.write(buffer, header.nbytes);
      writefile.close();
   }
}


void cix_rm (client_socket& server, string& nameOfFile) {
   // cout << "in cix_rm" << endl; // debugging tool

   // check size of filename
   if (nameOfFile.size() > 59) log << "Error: name of file, " << 
      nameOfFile << ", is too long." << endl;

   cix_header header;
   header.command = cix_command::RM;
   header.nbytes = 0;
   strcpy(header.filename, nameOfFile.c_str());

   // send header to server
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;

   if (header.command != cix_command::ACK) {
      log << "sent RM, server did not return ACK" << endl;
      log << "server returned " << header << endl;
   }
}


void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         log << "command " << line << endl;
         // split line by whitespace and retrieve filename
         string buf; // buffer string
         stringstream ss(line);  // insert the string into the stream
         vector<string> tokens;  // vector to hold words
         while (ss >> buf) tokens.push_back(buf);
         string nameOfFile = tokens.back();
         const auto& itor = command_map.find (tokens.front());
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::PUT:
               cix_put (server, tokens.back());
               break;
            case cix_command::GET:
               cix_get (server, tokens.back());
               break;
            case cix_command::RM:
               cix_rm (server, tokens.back());
               break;
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

