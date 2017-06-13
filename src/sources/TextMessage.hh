/*
Class:			TextMessage
Author:			Bjorn Tornqvist <bjorn@littlebig.rsn.hk-r.se>
Version:		1.0public
Last-Modified:	12th Jan 1999 by Bjorn Tornqvist
				19th Feb 1999 by Magnus Hjelmer
__________________________________________________________________________
Simple class to write and read the contents of a BMessage to a textfile.

Changes:
Made TextMessage work for both PPC and x86. You only have to add the correct
libraries to your project (if they aren't already there).

The following types are recognized:

B_BOOL_TYPE			A boolean value (the bool type)
B_INT8_TYPE			An 8-bit integer
B_INT16_TYPE		A 16-bit integer
B_INT32_TYPE		A 32-bit integer
B_INT64_TYPE		A 64-bit integer
B_FLOAT_TYPE		A float
B_DOUBLE_TYPE		A double
B_STRING_TYPE		A null-terminated character string

Usage:
* Copy the files TextMessage.cc and TextMessage.hh to your project
  folder (or a subfolder of it).
* Drag the file TextMessage.cc into your project.
* On PPC, make sure you have the following libraries included in your
  project (these are the ones in the sample project):
  	ppc/glue-nolimit.a
  	ppc/init_term_dyn.o
  	ppc/start_dyn.o
  	ppc/libbe.so
  	ppc/libroot.so
	ppc/libmslcpp_2_2.a
* On x86, make sure you have the following libraries included in your
  project (these are the ones in the sample project):
	x86/libstdc++.r4.so
	x86/libbe.so
* Make your calls to TextMessage.
* Compile and have fun.

This class is copyright (c) 1999 LittleBig Software Engineering
and is encouraged to be used widely by BeOS programmers out there
as long as credit is given where credit is due.
No warranties are given, I do, however, (unlike Java) think that this
class can safely be used in hospital (since it won't kill you) and in
army applications (wouldn't it be cool if this class helped shooting
down a scud? ;). Yea, right...
This class is NOT licensed under GPL - you don't have to publish any
changes made (as long as you give credit and keep this comment unmodified
here).
*/

#ifndef TEXTMESSAGE__HH__
#define TEXTMESSAGE__HH__

#include <app/Message.h>
#include <string>

#include <stdio.h>

class TextMessage : public BMessage
{
public:
	TextMessage(uint32);
//	TextMessage(BMessage*);
	TextMessage(const BMessage&);
	TextMessage();

	// Write a message to a file. If the message contains some parts
	// that are not handled by this class (such as pointers) they
	// will simply be ignored. Will return B_ERROR if an invalid
	// field was present in a name of a variable (disallowed sequences
	// include: '\t', "  ", and " = ". The string content of a variable
	// may not have '\r' or '\n' in it.
	// Returns B_OK if all is well.
	// Write this TextMessage (or <anotherMsg>) to the file in <thePath>:
	status_t WriteToTextFile(const char* thePath, const BMessage* anotherMsg=0) const;

	// Initialize this TextMessage (or <anotherMsg> BMessage) with the contents
	// from a file:
	void ReadFromTextFile(const char* thePath, BMessage* anotherMsg = 0);

	virtual ~TextMessage();

private:
	string myConvertTypeConstant(const uint32 theTypeCode) const;
	uint32 myConvertTypeConstant(const string) const;
};

#endif
