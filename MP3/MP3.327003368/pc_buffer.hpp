/* 
    File: pcbuffer.hpp

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2020/09/27

*/

#ifndef _pcbuffer_H_                   // include file only once
#define _pcbuffer_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include <iostream>
#include <string>
#include <pthread.h>
#include <queue>
#include "semaphore.hpp"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   bounded P C B u f f e r  */
/*--------------------------------------------------------------------------*/
template <class T>
class PCBuffer {

private:
  /* -- INTERNAL DATA STRUCTURES
     You will need to change them to fit your implementation. */

  // string        * buffer; // We buffer the data in an array of strings. 
                          // You may instead prefer a vector, or a queue, or ...
  int             size;   // Size of the bounded buffer.
  std::queue<T> q;
  Semaphore* ml;// mutex lock 
  Semaphore* len;// length 
  Semaphore* rem;// remaining

public:

  /* -- CONSTRUCTOR/DESTRUCTOR */

  PCBuffer(int _size){
    size =_size;
    ml = new Semaphore(1);
    len = new Semaphore(0);
    rem = new Semaphore(size);
  }

  ~PCBuffer(){
    delete ml;
    delete len;
    delete rem;
  }
  
  /* -- OPERATIONS ON PC BUFFER */

  int length() {
    return q.size();
  }

  void Deposit(T _item){
    // std::cout << "rem: " << rem->value << std::endl;
    rem->P();
    ml->P();
    q.push(_item);
    ml->V();
    len->V();
  }

  T Retrieve(){
    len->P();
    ml->P();
    T t = q.front();
    q.pop();
    ml->V();
    rem->V();
    return t;
  }

  void print(){
    std::queue<T> copy = q;
    while (!copy.empty()){
      std::cout << copy.front() << " ";
      copy.pop();
    }
  }
};


#endif


