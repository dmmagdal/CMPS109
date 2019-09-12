// $Id: file_sys.cpp,v 1.5 2016-01-14 16:16:52-08 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
         << ", prompt = \"" << prompt() << "\"");
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   // inode_ptr root = rootdirect;
   cwd = root;
   root->getContents()->insertInMap(".", root);
   root->getContents()->insertInMap("..", root);
   path.push_back("/");
}

const string& inode_state::prompt() { return prompt_; }

void inode_state::setPrompt(const string& newPrompt) {
   prompt_ = newPrompt+" ";
}

inode_ptr inode_state::getRoot(){
   return root;
}

inode_ptr inode_state::getCwd(){
   return cwd;
}

void inode_state::setCwd(inode_ptr newCwd){
   cwd = newCwd;
}

inode_ptr inode_state::getPrevCwd(){
   return prevCwd;
}

void inode_state::setPrevCwd (inode_ptr newPrevCwd){
   prevCwd = newPrevCwd;
}

wordvec inode_state::getPath (){
   return path;
}

void inode_state::append (const string& val){
   path.push_back(val);
}

void inode_state::pop () {
   path.pop_back();
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

int inode::size() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return contents->size();
}

inode_ptr inode::getNodePtr() const {
   return node_ptr;
}

base_file_ptr inode::getContents() const {
   return contents;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
   /*size_t size = 0;
   for (unsigned int i = 2; i != data.size(); ++i){
      size += data[i].length();
   }*/
   size_t size = data.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   data = words;
   DEBUGF ('i', words);
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&, const inode_ptr) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&, const wordvec&) {
   throw file_error ("is a plain file");
}

void plain_file::insertInMap (const string&, const inode_ptr) {
   throw file_error ("is a plain file");
}

map<string, inode_ptr> plain_file::returnMap (){
   throw file_error ("is a plain file");
}

bool plain_file::findFile (const string&) {
   throw file_error ("is a plain file");
}

bool plain_file::isADirectory (){
   return false;
}


size_t directory::size() const {
   size_t size = dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   if (dirents.find(filename) != dirents.end() and filename != "." and filename != ".."){
      if (dirents.find(filename)->second->getContents()->isADirectory() == true) {
         dirents.find(filename)->second->getContents()->returnMap().erase(".");
         dirents.find(filename)->second->getContents()->returnMap().erase("..");
      }
      dirents.erase(filename);
   }
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname, const inode_ptr parent) {
   inode_ptr subdirect = make_shared<inode>(file_type::DIRECTORY_TYPE);
   // dirents.insert(dirname, subdirect);
   insertInMap(dirname, subdirect);
   subdirect->getContents()->insertInMap(".", subdirect);
   subdirect->getContents()->insertInMap("..", parent);
   return subdirect;
}

inode_ptr directory::mkfile (const string& filename, const wordvec& words) {
   inode_ptr newfile = make_shared<inode>(file_type::PLAIN_TYPE);
   // insert key value pair into dirents
   insertInMap(filename, newfile);
   // write words to new file
   newfile->getContents()->writefile(words);
   DEBUGF ('i', filename);
   return newfile;
}

void directory::insertInMap (const string& key, const inode_ptr value){
   dirents.insert({key, value});
}

map<string, inode_ptr> directory::returnMap (){
   return dirents;
}

bool directory::findFile (const string& key){
   bool found = false;
   if (dirents.find(key) != dirents.end()){
      found = true;
   }
   return found;
}

bool directory::isADirectory (){
   return true;
}