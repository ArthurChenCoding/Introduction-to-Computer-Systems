/* 
    File: semaphore.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/11

*/

#ifndef _semaphore_H_                   // include file only once
#define _semaphore_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   S e m a p h o r e  */
/*--------------------------------------------------------------------------*/

class Semaphore {
private:
  /* -- INTERNAL DATA STRUCTURES
     You may need to change them to fit your implementation. */

  pthread_mutex_t m;
  pthread_cond_t  c;

public:
  int value;
  /* -- CONSTRUCTOR/DESTRUCTOR */

  Semaphore(int _val){
    pthread_mutex_init(&m,nullptr);
    pthread_cond_init(&c,nullptr);
    value = _val;
  }

  ~Semaphore(){
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&c);
  }

  /* -- SEMAPHORE OPERATIONS */

  int P(){
    pthread_mutex_lock(&m);
    while (value<=0) {//nasty bug missed the equal, reminder to test early
       pthread_cond_wait(&c,&m);
    }
    value--;
    pthread_mutex_unlock(&m);
    return 0;
  }
  int V(){
    pthread_mutex_lock(&m);
    value++;
    pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&m);
    return 0;
  }
};


#endif


