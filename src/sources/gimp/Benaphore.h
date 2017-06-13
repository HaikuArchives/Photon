/*--------------------------------------------------------------------*\
  File:      Benaphore.h
  Creator:   Matt Bogosian <mbogosian@usa.net>
  Copyright: (c)1997, 1998, Matt Bogosian. All rights reserved.
  Description: Header file describing the Benaphore class.
  ID:        $Id: Benaphore.h,v 1.29 1998/07/03 00:25:48 mattb Exp $
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
          (E.G., void myFunction(void);).
      Static member constants (declared with const) - begin with "mk_"
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

#include <OS.h>
#include <Debug.h>


#ifndef BENAPHORE_H
#define BENAPHORE_H


/*--------------------------------------------------------------------*\
  =-=-=-=-=-=-=-= Definitions, Enums, Typedefs, Consts =-=-=-=-=-=-=-=
\*--------------------------------------------------------------------*/

#ifndef DECL_SPEC

#if defined DLL_EXPORT_SYMBOLS
#define DECL_SPEC __declspec(dllexport)
#elif defined DLL_IMPORT_SYMBOLS
#define DECL_SPEC __declspec(dllimport)
#else
#define DECL_SPEC
#endif

#endif    // DECL_SPEC


/*--------------------------------------------------------------------*\
  =-=-=-=-=-=-=-=-=-=-=-=-= Structs, Classes =-=-=-=-=-=-=-=-=-=-=-=-=
\*--------------------------------------------------------------------*/

#ifdef DLL_EXPORT_SYMBOLS
#pragma export on
#endif

/*--------------------------------------------------------------------*\
  Class name:       Benaphore
  Inherits from:    none
  New data members: private static const char
                        mk_default_name[B_OS_NAME_LENGTH] - the
                        default name of the semaphore contained in the
                        Benaphore.
                    private sem_id m_sem - the semaphore used in the
                        Benaphore.
                    private int32 m_ben_cnt - the count of entities
                        who have aquired or are waiting to aquire the
                        Benaphore.
  Description: Class to implement a signal-safe "Benaphore" variant
      semaphore for critical section mutex-wrapping. Benaphores are
      described in issue #26 of the Be Newsletter <http://www.be.com/
      aboutbe/benewsletter/Issue26.html> and the BeOS Kernel Kit API
      <http://www.be.com/documentation/be_book/kernel/sems.html
      #Benaphores>.
      
      The implementation of a Benaphore described in both the Be
      Newsletter and the BeOS Kernel Kit API is not signal-safe; if a
      thread waiting on acquiring a Benaphore received a signal, the
      Benaphore would fail, and may enter a state in which the
      Benaphore became invalid. This implementation corrects that
      problem by catching the value returned by acquire_sem_etc() and
      trying again if that value is B_INTERRUPTED. As a result of this
      change, if a thread receives a signal while waiting for a
      Benaphore, it may be moved to the end of the line to acquire it.
      
      To use this class, simply instatiate it, and then, when a thread
      wishes to enter a critical section, all it has to do is call
      Lock() and Unlock():
      
        if (my_benaphore.Lock() == B_NO_ERROR)
        {
          // Critical code goes here
          
          my_benaphore.Unlock();
        }
        else
        {
          // Don't do anything important
        }
      
      Note: you still must check the return value of Lock() for
      success (just like the acquire_sem*() functions). Calling Lock()
      with its default arguments is equivalent to using acquire_sem()
      (as opposed to acquire_sem_etc()) to acquire the semaphore.
\*--------------------------------------------------------------------*/

class DECL_SPEC Benaphore
{
	private:
	
		// Private static const data members
		static const char mk_default_name[B_OS_NAME_LENGTH];
	
	public:
	
		// Public member functions
		
/*--------------------------------------------------------------------*\
  Function name: inline Benaphore
  Member of:     public Benaphore
  Arguments:     const char a_name[B_OS_NAME_LENGTH] - the name of the
                     semaphore associated with the Benaphore. Default:
                     mk_default_name.
                 status_t * const a_err - a placeholder for the error
                     encountered during construction (see below).
                     Default: NULL.
  Throws:        none
  Description: Class constructor. You must check the value of a_err or
      the return value of Status() after invoking the constructor to
      see if the object is usable. See the Status() function (below)
      for possible error values.
\*--------------------------------------------------------------------*/
		
		inline Benaphore(const char a_name[B_OS_NAME_LENGTH] = mk_default_name, status_t * const a_err = NULL) :
			m_sem(create_sem(0, a_name)),
			m_ben_cnt(0)
		{
			// Give the error back to the caller
			if (a_err != NULL)
			{
				*a_err = Status();
			}
		}
		
/*--------------------------------------------------------------------*\
  Function name: inline ~Benaphore
  Member of:     public Benaphore
  Arguments:     none
  Throws:        none
  Description: Class destructor.
\*--------------------------------------------------------------------*/
		
		~Benaphore(void);
		
/*--------------------------------------------------------------------*\
  Function name: inline Count const
  Member of:     public Benaphore
  Arguments:     none
  Returns:       int32 - the thread count.
  Throws:        none
  Description: Function to return the number of callers attempting to
      lock the Benaphore. Note: this is probably as (if not less)
      reliable as get_sem_count() kernel function. As said in the BE
      BOOK, "never predicate your code on this function".
\*--------------------------------------------------------------------*/
		
		inline int32 Count(void) const
		{
			return m_ben_cnt;
		}
		
/*--------------------------------------------------------------------*\
  Function name: inline Info const
  Member of:     public Benaphore
  Arguments:     sem_info &a_info - the semaphore's info.
  Returns:       status_t - the error code associated with the
                     operation (see below).
  Throws:        none
  Description: Function to return the info about the semaphore
      associated with this Benaphore. This should be used for
      informative purposes only. Possible return values are as
      follows:
      
      B_NO_ERROR - success.
      B_BAD_SEM_ID - this Benaphore no longer contains a valid
          semaphore.
\*--------------------------------------------------------------------*/
		
		inline status_t Info(sem_info &a_info) const
		{
			return get_sem_info(m_sem, &a_info);
		}
		
/*--------------------------------------------------------------------*\
  Function name: Lock
  Member of:     public Benaphore
  Defined in:    Benaphore.cpp
  Arguments:     const uint32 a_flags - the flags passed to
                     acquire_sem_etc(). Default: 0.
                 const bigtime_t a_timeout - the timeout in
                     microseconds. Default: B_INFINITE_TIMEOUT.
  Returns:       status_t - the error code associated with the
                     operation (see below).
  Throws:        none
  Description: Function to aquire the Benaphore. If the Benaphore is
      valid, then this function will block until the Benaphore is
      locked or until another condition (defined by the arguments) is
      reached, at which time the function will return the status. A
      successful call to Lock() (i.e., on which returns B_NO_ERROR)
      should always be mirrored by exactly one call to Unlock().
      Possible return values are as follows:
      
      B_NO_ERROR - success.
      B_BAD_SEM_ID - this Benaphore is not valid.
      B_WOULD_BLOCK - a timeout of 0 was specified and the Benaphore
          was not available.
      B_TIMED_OUT - the timeout was reached and the Benaphore was not
          acquired.
\*--------------------------------------------------------------------*/
		
		status_t Lock(const uint32 a_flags = 0, const bigtime_t a_timeout = B_INFINITE_TIMEOUT);
		
/*--------------------------------------------------------------------*\
  Function name: inline SetOwner
  Member of:     public Benaphore
  Arguments:     const team_id a_team - the team to which to transfer
                     ownership.
  Returns:       status_t - the error code associated with the
                     operation (see below).
  Throws:        none
  Description: Function to transfer the ownership of the Benaphore to
      another team. Possible return values are as follows:
      
      B_NO_ERROR - success.
      B_BAD_SEM_ID - either the caller does not have ownership of the
          Benaphore's semaphore, or this Benaphore no longer contains
          a valid semaphore.
      B_BAD_TEAM_ID - the specified team was not valid.
\*--------------------------------------------------------------------*/
		
		inline status_t SetOwner(const team_id a_team)
		{
			return set_sem_owner(m_sem, a_team);
		}
		
/*--------------------------------------------------------------------*\
  Function name: inline Status const
  Member of:     public Benaphore
  Arguments:     none
  Returns:       status_t - B_NO_ERROR if the object is valid, another
                     error code if not (see below).
  Throws:        none
  Description: Function to check if the object is valid. This function
      must be called after creating the object to insure that its
      instantiation was successful. Possible return values are as
      follows:
      
      B_NO_ERROR - the object was created successfully.
      B_NO_MEMORY - on construction, there was not enough memory
          available to create the object.
      B_NO_MORE_SEMS - on construction, there were no available
          semaphores to create the object.
\*--------------------------------------------------------------------*/
		
		inline status_t Status(void) const
		{
			return (m_sem >= B_NO_ERROR) ? B_NO_ERROR : m_sem;
		}
		
/*--------------------------------------------------------------------*\
  Function name: inline Unlock
  Member of:     public Benaphore
  Defined in:    Benaphore.cpp
  Arguments:     const uint32 a_flags - the flags passed to
                     release_sem_etc(). Default: 0.
  Returns:       none
  Throws:        none
  Description: Function to release the Benaphore. There should be
      exactly one call to Unlock() for every successful call to
      Lock().
\*--------------------------------------------------------------------*/
		
		inline void Unlock(const uint32 a_flags = 0)
		{
			// Decrement the benaphore count and release
			// the semaphore if necessary. Note:
			// atomic_add() returns the previous value of
			// m_ben_cnt.
			if (atomic_add(&m_ben_cnt, -1) > 1)
			{
				release_sem_etc(m_sem, 1, a_flags);
			}
		}
	
	private:
	
		// Private data members
		sem_id m_sem;
		int32 m_ben_cnt;
		
		// Private prohibitted member functions
		Benaphore(const Benaphore &a_obj);
		Benaphore &operator=(const Benaphore &a_obj);
};

class AutoLockBenaphore {
public:
			AutoLockBenaphore (Benaphore *zLock) { fLock = zLock; fLock->Lock (); }
			~AutoLockBenaphore () { fLock->Unlock (); }
protected:
	Benaphore	*fLock;
};

#ifdef DLL_EXPORT_SYMBOLS
#pragma export off
#endif


#endif    // BENAPHORE_H
