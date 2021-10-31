/* 
    File: simpleclient.cpp

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2019/09/23

    Simple client main program for MP2 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <errno.h>
#include <unistd.h>

#include "pcbuffer.hpp"
#include "reqchannel.hpp"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

std::string int2string(int number) {
  std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

std::string generate_data() {
  // Generate the data to be returned to the client.
  return int2string(rand() % 100);
}

std::string process_request(const std::string & _request) {

  if (_request.compare(0, 5, "hello") == 0) {
    return "hello to you, too";
  }
  else if (_request.compare(0, 4, "data") == 0) {
    return generate_data();
  }
  else {
    return "unknown request";
  }

}

typedef struct{
  int n_req;
  std::string patient_name;
  PCBuffer<std::string>* PCB;
}RTFArgs;

void* request_thread_func(void* args){
  std::cout << "starting r thread" << std::endl;
  RTFArgs* rtfa = (RTFArgs*) args;
  for(int i = 0; i < rtfa->n_req; i++) {

  std::cout << "done" << std::endl;
    std::string req = "data" + rtfa->patient_name;

  std::cout << "done" << std::endl;
    rtfa->PCB->Deposit(req);
  std::cout << "done" << std::endl;
  }
  std::cout<<"all jobs are done by r_thread"<<std::endl;
}

typedef struct{
  PCBuffer<std::string>* PCB;
}WTFArgs;

void* worker_thread_func(void* args){
  std::cout << "starting w thread" << std::endl;
  WTFArgs* wtfa = (WTFArgs*) args;
  for(;;) {
    std::string req = wtfa->PCB->Retrieve();
    std::cout<<"retrieved req = "<<req<<std::endl;
  }
}

typedef struct{
  int counter = 0;
  std::string patient_name;
  PCBuffer<std::string>* PCB;
}STFArgs;

/*
void* stat_thread_func(void* args) {
  STFArgs* stfa = (STFAArgs*) args;
  // int req_id = *((int*)person_id);

  Response r("dummy",-1,-1);

  for(int i=0; i<n_requests; i++) {
    if (req_id==0){
        r = joe_buf->retrieve_response();
        joe_histogram[atoi(r.str.c_str())]+=1; //increment corresponding integer of the histogram
    }else if (req_id ==1){
             r = jane_buf->retrieve_response();
             jane_histogram[atoi(r.str.c_str())]+=1;
    }else if (req_id==2){
                 r = john_buf->retrieve_response();
                 john_histogram[atoi(r.str.c_str())]+=1;

    }
  }
  cout << "stat thread finished " << req_id<< endl;
}
*/

void printB(PCBuffer<std::string> arg){
  arg.print();
}
/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

/* client 
  -n <number of data requests per patient>
  -b <size of bounded buffer between request and worker threads>
  -w <number of worker threads>
*/

// defualt arguments
int n_req = 100;
int n_work = 10;
int b_size = 50;

// PCBuffer<string>* PCB;
// PCBuffer<string>* Bjoe;
// PCBuffer<string>* Bjane;
// PCBuffer<string>* Bjohn;

int main(int argc, char * argv[]) {
  int option = 0;
  while((option = getopt(argc, argv, "n:b:w:"))!= -1) {
    if(option == 'n') {
      n_req = atoi(optarg);
      break;
    }else if(option == 'b') {
      b_size = atoi(optarg);
      break;
    }else if(option == 'w') {
      n_work = atoi(optarg);
      break;
    }else {
      std::cout<<"invalid command line argument"<<std::endl;
      assert(false);
    }
  }
  // initializations
  pthread_t r_threads[3];
  pthread_t w_threads[n_work];
  // pthread_t s_threads[3];

  // Bjoe= new PCBuffer<string>(b_size);
  // RTFArgs* Rjoe;
  // Rjoe->n_req = n_req;
  // Rjoe->patient_name = "joe";
  // Rjoe->PCB = Bjoe;
  PCBuffer<std::string>* PCB= new PCBuffer<std::string>(b_size);
  // WTFArgs* WB;
  // WB->PCB = new PCBuffer<string>(b_size);
  // Bjane= new PCBuffer<string>(b_size);
  // Bjohn= new PCBuffer<string>(b_size);

  int* zero = new int(0);
  // int* one = new int(1);
  // int* two = new int(2);

  // pid_t pid;
  // if ((pid = fork()) == 0) {
  //   // child process for server
  //   // system("./dataserver");
  // }else {
    std::cout << "CLIENT STARTED:" << std::endl;

    std::cout << "Establishing control channel... " << std::flush;
    // RequestChannel chan("control", RequestChannel::Side::CLIENT);
    std::cout << "done." << std::endl;

    /* -- Start sending a sequence of requests */
    timeval start1, end1;
    gettimeofday(&start1, nullptr);
    
    std::cout<<"start to create request threads"<<std::endl;
    pthread_create(&r_threads[0], nullptr, request_thread_func, (void*)PCB);
    // pthread_create(&r_threads[1], nullptr, request_thread_func, (void*)one);
    // pthread_create(&r_threads[2], nullptr, request_thread_func, (void*)two);
    pthread_join(r_threads[0],nullptr);
    std::cout<<"start to create worker threads"<<std::endl;
    //for(int i = 0; i < n_work; i++) {
      // string reply = chan.send_request("newthread");
      // RequestChannel* channel = new RequestChannel(reply,RequestChannel::Side::CLIENT);
      pthread_create(&w_threads[0], nullptr, worker_thread_func, (void*)PCB);
    //}
    // std::cout << "Creating stat threads"<<std::endl;
    // pthread_create(&s_threads[0], nullptr, s_thread, (void*)zero);
    // pthread_create(&s_threads[1], nullptr, s_thread, (void*)one);
    // pthread_create(&s_threads[2], nullptr, s_thread, (void*)two);
    
    pthread_join(r_threads[0],nullptr);
    //std::string reply2 = chan.send_request("data Joe Smith");
    
    //std::cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << std::endl;

    
    //std::string reply3 = chan.send_request("data Jane Smith");
    
    //std::cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << std::endl;

    // for(int i = 0; i < 100; i++) {
    //   std::string request_string("data TestPerson" + int2string(i));
      // std::string reply_string = chan.send_request(request_string);
      // std::cout << "reply to request " << i << ":" << reply_string << std::endl;;
    //}

    // /* -- Start meassuring the local call */
    // timeval Start1, End1, Start2, End2, Start3, End3;

    // gettimeofday(&Start1, nullptr);
    // std::string Reply1 = process_request("hello");
    // gettimeofday(&End1, nullptr);

    // gettimeofday(&Start2, nullptr);
    // std::string Reply2 = process_request("data Joe Smith");
    // gettimeofday(&End2, nullptr);

    // gettimeofday(&Start3, nullptr);
    // std::string Reply3 = process_request("data Jane Smith");
    // gettimeofday(&End3, nullptr);
    // /* -- End meassuring the local call */

    
    // std::string reply4 = chan.send_request("quit");
    
    // std::cout << "Reply to request 'quit' is '" << reply4 << std::endl;

    // std::cout<<"Time spend on server to send hello is "<<end1.tv_sec-start1.tv_sec<<" secs and "<<end1.tv_usec-start1.tv_usec<<" musecs."<<std::endl;
    // std::cout<<"Time spend on server to send data Joe Smith is "<<end2.tv_sec-start2.tv_sec<<" secs and "<<end2.tv_usec-start2.tv_usec<<" musecs."<<std::endl;
    // std::cout<<"Time spend on server to send data Jane Smith is "<<end3.tv_sec-start3.tv_sec<<" secs and "<<end3.tv_usec-start3.tv_usec<<" musecs."<<std::endl;
    // std::cout<<"Time spend on server to send quit is "<<end4.tv_sec-start4.tv_sec<<" secs and "<<end4.tv_usec-start4.tv_usec<<" musecs."<<std::endl;
    // std::cout<<std::endl<<"Meassurements of local calls:"<<std::endl;
    // std::cout<<"Time spend on server to send hello is "<<End1.tv_sec-Start1.tv_sec<<" secs and "<<End1.tv_usec-Start1.tv_usec<<" musecs."<<std::endl;
    // std::cout<<"Time spend on server to send data Joe Smith is "<<End2.tv_sec-Start2.tv_sec<<" secs and "<<End2.tv_usec-Start2.tv_usec<<" musecs."<<std::endl;
    // std::cout<<"Time spend on server to send data Jane Smith is "<<End3.tv_sec-Start3.tv_sec<<" secs and "<<End3.tv_usec-Start3.tv_usec<<" musecs."<<std::endl;
    std::cout << "done" << std::endl;
    // usleep(1000000);
  // }
}
