// $Id: commands.cpp,v 1.16 2016-01-14 16:10:40-08 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() == 2) {
      wordvec targetdirect = split (words.at(1), "/");
      state.setPrevCwd(state.getRoot());
      // iterate throw path to get to proper directoru
      for (unsigned int i = 0; i != targetdirect.size()-1; ++i){
         if (state.getPrevCwd()->getContents()->findFile(targetdirect.at(i)) == false){
            throw command_error("Path does not exist");
            state.setPrevCwd(nullptr);
            break;
         }
         else {
            state.setPrevCwd(state.getPrevCwd()->getContents()->returnMap().find(targetdirect.at(i))->second);
         }
      }

      string targetfile = targetdirect.at(targetdirect.size()-1);

      // now in the correct directory
      // look for file to read
      if (state.getPrevCwd()->getContents()->findFile(targetfile) == true and state.getPrevCwd()->getContents()->returnMap().find(targetfile)->second->getContents()->isADirectory() == false){
         //if (state.getPrevCwd()->getContents()->returnMap().fin)
         cout << state.getPrevCwd()->getContents()->returnMap().find(targetfile)->second->getContents()->readfile() << endl;
      }
      else {
         throw command_error("No such file or directory");
      }

      // clear prevCwd
      state.setPrevCwd(nullptr);
   }
   else {
      throw command_error("Missing argument.");
   }

   /*
   auto i = state.getCwd()->getContents()->returnMap().find(words.at(1));
   // find file
   if (i == state.getCwd()->getContents()->returnMap().end() or i->second->getContents()->isADirectory() == true){
      cout << "cat: " << words << ": No such file or directory." << endl;
   }
   else {
      cout << i->second->getContents()->readfile();
   }*/
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (state.getCwd()->getContents()->returnMap().find(words.at(1)) == state.getCwd()->getContents()->returnMap().end()){
      throw command_error("No such directory exists");
   }
   else if (state.getCwd()->getContents()->returnMap().find(words.at(1))->second->getContents()->isADirectory() == false){
      throw command_error("Can't call command 'cd' on a file");
   }
   else {
      if (words.at(1).compare("..") == 0 and words.size() > 1){
         state.pop();
      }
      else if (words.at(1).compare(".") != 0){
         state.append(words.at(1));
      }
      state.setCwd(state.getCwd()->getContents()->returnMap().find(words.at(1))->second);
   }
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // if there is a path name specified
   if (words.size() > 1){
      wordvec path {"/"};
      wordvec targetdirect = split (words.at(1), "/");
      state.setPrevCwd(state.getRoot());
      // iterate throw path to get to proper directory
      for (unsigned int i = 0; i != targetdirect.size(); ++i){
         if (state.getPrevCwd()->getContents()->findFile(targetdirect.at(i)) == false){
            throw command_error("path does not exist.");
            state.setPrevCwd(nullptr);
            break;
         }
         else {
            path.push_back(targetdirect.at(i));
            state.setPrevCwd(state.getPrevCwd()->getContents()->returnMap().find(targetdirect.at(i))->second);
         }
      }

      // now in the correct directory
      // print cwd 
      cout << word_range(path.cbegin() + 1, path.cend()) << ":" << endl;
      // print the cwd map
      for (auto i = state.getPrevCwd()->getContents()->returnMap().cbegin(); i != state.getPrevCwd()->getContents()->returnMap().cend(); ++i){
         if (i->second->getContents()->isADirectory() == true and i->first.compare(".") != 0 and i->first.compare("..") != 0){
            cout << i->second->get_inode_nr() << "  " << i->second->size() << " " << i->first << "/" << endl;
         }
         else {
            cout << i->second->get_inode_nr() << "  " << i->second->size() << " " << i->first << endl;
         }
      }

      // clear prevCwd
      state.setPrevCwd(nullptr);
   }
   else {
      // print path to cwd
      for (unsigned int i = 0; i != state.getPath().size(); ++i){
         if (i == state.getPath().size()-1){
            cout << state.getPath().at(i) << endl;
         }
         else if (i == 0){
            cout << state.getPath().at(i);
         }
         else {
            cout << state.getPath().at(i) << "/";
         }
      }
      // print the cwd map
      for (auto i = state.getCwd()->getContents()->returnMap().cbegin(); i != state.getCwd()->getContents()->returnMap().cend(); ++i){
         if (i->second->getContents()->isADirectory() == true and i->first.compare(".") != 0 and i->first.compare("..") != 0){
            cout << i->second->get_inode_nr() << "  " << i->second->size() << " " << i->first << "/" << endl;
         }
         else {
            cout << i->second->get_inode_nr() << "  " << i->second->size() << " " << i->first << endl;
         }
      }
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   fn_ls(state, words);

   for (auto i = state.getCwd()->getContents()->returnMap().cbegin(); i != state.getCwd()->getContents()->returnMap().cend(); ++i){
      if (i->first.compare(".") != 0 or i->first.compare("..") != 0){
         if (i->second->getContents()->isADirectory() == true){
            wordvec nextdir;
            nextdir.push_back(" ");
            nextdir.push_back(i->first);
            fn_cd(state, nextdir);
            fn_lsr (state, words);
            nextdir.pop_back();
            nextdir.push_back("..");
            fn_cd(state, words);
         }
      }
      else {
         continue;
      }
   }
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   wordvec alteredWord = words;
   alteredWord.erase(alteredWord.cbegin());   // fn_command
   alteredWord.erase(alteredWord.cbegin());   // filename
   if (words.size() > 1){
      if (state.getCwd()->getContents()->findFile(words.at(1)) == true){
         state.getCwd()->getContents()->returnMap().find(words.at(1))->second->getContents()->writefile(alteredWord);
      }
      else {
         state.getCwd()->getContents()->mkfile(words.at(1), alteredWord);
      }
   }
   else {
      throw command_error("Error: no file to make.");
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // if there is a path name specified
   if (words.size() == 2){
      wordvec targetdirect = split (words.at(1), "/");
      state.setPrevCwd(state.getRoot());
      // iterate throw path to get to proper directoru
      for (unsigned int i = 0; i != targetdirect.size()-1; ++i){
         if (state.getPrevCwd()->getContents()->findFile(targetdirect.at(i)) == false){
            throw command_error("path does not exist.");
            state.setPrevCwd(nullptr);
            break;
         }
         else {
            state.setPrevCwd(state.getPrevCwd()->getContents()->returnMap().find(targetdirect.at(i))->second);
         }
      }

      string newdirect = targetdirect.at(targetdirect.size()-1);

      // now in correct directory
      // add dir procedure
      if (state.getPrevCwd()->getContents()->findFile(newdirect) == false){
         state.getPrevCwd()->getContents()->mkdir(newdirect, state.getPrevCwd());
      }
      else {
         throw command_error("Can not have directory with existing name.");
      }

      // clear prevCwd
      state.setPrevCwd(nullptr);
   }
   else {
      throw command_error("Missing argument.");
      /*
      // add dir procedure
      if (state.getCwd()->getContents()->findFile(words.at(1)) == false){
         state.getCwd()->getContents()->mkdir(words.at(1), state.getCwd());
      }
      else {
         throw command_error("Can not have directory with existing name.");
      }*/
   }
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() > 1){
      state.setPrompt(words.at(1));
   }
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   for (unsigned int i = 0; i != state.getPath().size(); ++i){
      if (i == state.getPath().size()-1){
         cout << state.getPath().at(i) << endl;
      }
      else if (i == 0){
         cout << state.getPath().at(i);
      }
      else {
         cout << state.getPath().at(i) << "/";
      }
   }
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // if there is a path name specified
   if (words.size() == 2){ 
      wordvec targetdirect = split (words.at(1), "/");
      state.setPrevCwd(state.getRoot());
      // iterate throw path to get to proper directory
      for (unsigned int i = 0; i != targetdirect.size()-1; ++i){
         if (state.getPrevCwd()->getContents()->findFile(targetdirect.at(i)) == false){
            throw command_error("path does not exist.");
            state.setPrevCwd(nullptr);
            break;
         }
         else {
            state.setPrevCwd(state.getPrevCwd()->getContents()->returnMap().find(targetdirect.at(i))->second);
         }
      }

      string target = targetdirect.at(targetdirect.size()-1);

      // now in the correct directory
      if (state.getPrevCwd()->getContents()->findFile(target) == false){
         throw command_error("File or directory not found.");
      }
      else if (state.getPrevCwd()->getContents()->returnMap().find(target)->second->getContents()->isADirectory() == true and state.getPrevCwd()->getContents()->returnMap().find(target)->second->size() > 2){
         throw command_error("Directory must be empty before removal.");
      }
      else {
         state.getPrevCwd()->getContents()->remove(target);
      }

      // clear prevCwd
      state.setPrevCwd(nullptr);
   }
   else {
      throw command_error("Missing argument.");
   }
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() == 2){
      wordvec targetdirect = split (words.at(1), "/");
      state.setPrevCwd(state.getRoot());
      // iterate throw path to get to proper directory
      for (unsigned int i = 0; i != targetdirect.size()-1; ++i){
         if (state.getPrevCwd()->getContents()->findFile(targetdirect.at(i)) == false){
            throw command_error("path does not exist.");
            state.setPrevCwd(nullptr);
            break;
         }
         else {
            state.setPrevCwd(state.getPrevCwd()->getContents()->returnMap().find(targetdirect.at(i))->second);
         }
      }

      string target = targetdirect.at(targetdirect.size()-1);

      // now in the correct directory
      if (state.getPrevCwd()->getContents()->findFile(target) == false){
         throw command_error("File or directory not found.");
      }
      else if (state.getPrevCwd()->getContents()->returnMap().find(target)->second->getContents()->isADirectory() == true and state.getPrevCwd()->getContents()->returnMap().find(target)->second->size() > 2){
         
      }
      else {
         state.getPrevCwd()->getContents()->remove(target);
      }

      // clear prevCwd
      state.setPrevCwd(nullptr);
   }
   else {
      throw command_error("Missing argument.");
   }
}

