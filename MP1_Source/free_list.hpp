/* 
    File: free_list.hpp

    Author: Arthur Chen
            Department of Computer Science and Engineering
            Texas A&M University
    Date  : Aug 28th

    Modified:

*/

#ifndef _free_list_hpp_                   // include file only once
#define _free_list_hpp_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

class SegmentHeader {

private:

  static const unsigned COOKIE_VALUE = 0xBADB00;
  unsigned int cookie; /* To check whether this is a genuine header! */
  bool is_free;

public:

  SegmentHeader * previous;
  SegmentHeader * next;
  size_t length;
  int index;
  int type;
  int inh;
  SegmentHeader(size_t _length, bool _is_free = true);
  
  ~SegmentHeader();
  /* We probably won't need the destructor. */

  void CheckValid();
  /* Check if the cookie is valid. */

  SegmentHeader * Split(size_t);
  
  bool isFree();
  // size_t getLength(); 

  // void changeLength(size_t);

  // SegmentHeader* getNext();

  void PrintSeg();

  void setFree(bool);
};

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS  FreeList */
/*--------------------------------------------------------------------------*/

class FreeList {

 private:
 
  SegmentHeader* head;
  SegmentHeader* tail;

public:
  bool isEmpty();

  FreeList(); 
  /* This function initializes a new free-list. */

  ~FreeList(); 
  /* We probably don't need a destructor. */ 

  bool Remove(SegmentHeader * _segment); 
  /* Remove the given segment from the given free list. 
     Returns true if the function succeeds.
  */ 

  bool Add(SegmentHeader * _segment); 
  /* Add the segment to the given free list. */
  
  SegmentHeader* getHead();

  // function to print every sh (ifFreed, length, and start address) of freelist 
  void PrintFreeList();
};

#endif 
