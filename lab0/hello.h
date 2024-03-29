// $Id: hello.h,v 1.1 2018-01-08  

#ifndef __HELLO_H__
#define __HELLO_H__

#include <iostream>
#include <string>

class hello{
	private:
		std::string message {"Hello, World!"};
	public:
		hello();									// default constructor
		hello (const hello&);						// copy constructor
		hello& operator= (const hello&);			// copy operator=
		hello (hello&&);							// move constructor
		hello& operator= (hello&&);					// move operator= 
		~hello();									// destructor
		hello (const string&);						// alternative constructor
		void say (ostream&);						// member function
};

#endif
