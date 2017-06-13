/*--------------------------------------------------------------------*\
  File:      Benaphore.cpp
  Creator:   Matt Bogosian <mbogosian@usa.net>
  Copyright: (c)1997, 1998, Matt Bogosian. All rights reserved.
  Description: Source file containing the member functions for the
      Benaphore class.
  ID:        $Id: Benaphore.cpp,v 1.21 1998/07/01 05:16:16 mattb Exp $
  Conventions:
      #defines - all uppercase letters with words separated by
          underscores.
          (E.G., #define MY_DEFINE 16).
      New data types (classes, structs, typedefs, etc.) - begin with
          an uppercase letter followed by lowercase words separated by
          uppercase letters.
          (E.G., typedef int MyTypedef;).
      Global constants (declared with const) - begin with "k_"
          followed by lowercase words separated by underscores.
          (E.G., const int k_my_constant(16);).
      Global variables - begin with "g_" followed by lowercase words
          separated by underscores.
          (E.G., int g_my_global;).
      Local variables - begin with a lowercase letter followed by
          lowercase words separated by underscores.
          (E.G., int my_local;).
      Argument variables - begin with "a_" followed by lowercase words
          separated by underscores.
          (E.G., ..., int a_my_argument, ...).
      Functions - begin with a lowercase letter followed by lowercase
          words separated by uppercase letters.
          (E.G., void myFunction(void);).      Static member constants (declared with const) - begin with "mk_"
          followed by lowercase words separated by underscores.
          (E.G., static const int MyClass::mk_my_constant(16);).
      Static member variables - begin with "mg_" followed by lowercase
          words separated by underscores.
          (E.G., static int MyClass::mg_my_global(16);).
      Member variables - begin with "m_" followed by lowercase words
          separated by underscores.
          (E.G., int m_my_member;).
      Member Functions - begin with an uppercase letter followed by
          lowercase words separated by uppercase letters.
          (E.G., void MyClass::MyFunction(void);).
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
  =-=-=-=-=-=-=-=-=-=-=-=-=- Included Files -=-=-=-=-=-=-=-=-=-=-=-=-=
\*--------------------------------------------------------------------*/

#include "Benaphore.h"


/*--------------------------------------------------------------------*\
  =-=-=-=-=-=-=-= Definitions, Enums, Typedefs, Consts =-=-=-=-=-=-=-=
\*--------------------------------------------------------------------*/

// Initialize the Benaphore private static const data members
const char Benaphore::mk_default_name[B_OS_NAME_LENGTH] = "Benaphore";


/*--------------------------------------------------------------------*\
  =-=-=-=-=-=-=-=-=-=-=-= Function Definitions =-=-=-=-=-=-=-=-=-=-=-=
\*--------------------------------------------------------------------*/

//====================================================================
status_t Benaphore::Lock(const uint32 a_flags, const bigtime_t a_timeout)
//====================================================================
{
	// Check to see if the Benaphore is valid
	if (m_sem < B_NO_ERROR)
	{
		return B_BAD_SEM_ID;
	}
	
	// Increment the benaphore count and acquire the semaphore if
	// necessary. Note: atomic_add() returns the previous value of
	// m_ben_cnt.
	status_t err(B_NO_ERROR);
	
	if (atomic_add(&m_ben_cnt, 1) > 0)
	{
		while ((err = acquire_sem_etc(m_sem, 1, a_flags, a_timeout)) == B_INTERRUPTED)
		{
			;
		}
	}
	
	// Don't do anything if m_sem doesn't represent a valid
	// semaphore (it probably means we got deleted), but back out
	// on another kind of failure
	if (err != B_BAD_SEM_ID
		&& err != B_NO_ERROR)
	{
		atomic_add(&m_ben_cnt, -1);
		
		// Consolidate the return values a little
		if (err != B_WOULD_BLOCK
			&& err != B_TIMED_OUT)
		{
			err = B_BAD_SEM_ID;
		}
	}
	
	return err;
}

Benaphore::~Benaphore(void) {
	Lock ();
	ASSERT (m_ben_cnt == 1);
	delete_sem(m_sem);
}
