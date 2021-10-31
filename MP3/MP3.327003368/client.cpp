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

#include "pc_buffer.hpp"
#include "reqchannel.hpp"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    // defualt arguments
    int n_req = 100;
    int n_work = 5;
    int b_size = 50;

    PCBuffer<std::string>* joe_b;
    PCBuffer<std::string>* jane_b;
    PCBuffer<std::string>* john_b;

    std::queue<std::string> joe_q;
    std::queue<std::string> jane_q;
    std::queue<std::string> john_q;

    pthread_t s_threads[3];

    bool wNotDone = true;

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
  // std::cout << "starting r thread" << std::endl;
  RTFArgs* rtfa = (RTFArgs*) args;
  for(int i = 0; i < rtfa->n_req; i++) {
    std::string req = "data " + rtfa->patient_name;
    rtfa->PCB->Deposit(req);
    // printf("deposited %s \n", req.c_str());
  }
  // std::cout<<"all jobs are done by r_thread"<<std::endl;
  return rtfa;
}

typedef struct{
  PCBuffer<std::string>* PCB;
  RequestChannel* rc;
}WTFArgs;
// int wcount =0;
void* worker_thread_func(void* args){
  // std::cout << "starting w thread" << std::endl;
  WTFArgs* wtfa = (WTFArgs*) args;
  for(;;) {
    std::string req = wtfa->PCB->Retrieve();
    // printf("retrieved req = %s. \n",req.c_str());
    if(req == "quit") {
      // std::cout<<"quit recieved from worker"<<std::endl;
      break; // exit
    }
    // wcount++;
    // printf("work threads had processed %d req in total", wcount);
    std::string reply = wtfa->rc->send_request(req); // send a request using WTFArgs->rc to the dataserver
    // printf("response from the server is %s\n",reply.c_str()); // Print response
    // fine patient from req then deposit into stat PCB 
    if (req.compare(5, 8, "joe") == 0) {
      joe_b->Deposit(reply);
    }else if (req.compare(5, 9, "jane") == 0) {
      jane_b->Deposit(reply);
    }else if (req.compare(5, 9, "john") == 0) {
      john_b->Deposit(reply);
    }else {
      std::cout<<"unknown req from work_thread"<<std::endl;
    } 
  }
  // std::cout<<"w thread has done its job"<<std::endl;
  return wtfa;
}

typedef struct{
  int counter = 0;
  std::string patient_name;
  PCBuffer<std::string>* PCB;
}STFArgs;

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

// print out histogram for each of the patient by freq
void printH(std::queue<std::string> q){
    int cs[10] = {0,0,0,0,0,0,0,0,0,0};//counters, 0-10, 11-20....
    std::queue<std::string> copy = q;

    while (!copy.empty()){
      if (!is_number(copy.front())){
        std::cout<<"error in printh"<< std::endl;
        assert(false);
      }
      int temp = std::stoi(copy.front());
      if (0<=temp && temp <= 10){
        cs[0]++;
      }else if (11<=temp && temp <= 20){
        cs[1]++;
      }else if (21<=temp && temp <= 30){
        cs[2]++;
      }else if (31<=temp && temp <= 40){
        cs[3]++;
      }else if (41<=temp && temp <= 50){
        cs[4]++;
      }else if (51<=temp && temp <= 60){
        cs[5]++;
      }else if (61<=temp && temp <= 70){
        cs[6]++;
      }else if (71<=temp && temp <= 80){
        cs[7]++;
      }else if (81<=temp && temp <= 90){
        cs[8]++;
      }else if (91<=temp && temp <= 100){
        cs[9]++;
      }else{
        std::cout<<"number invialid!!"<<std::endl;
      }
      copy.pop();
    }
      std::cout<<std::setw(7)<<"0-9"<<std::setw(7)<<"10-19"<<std::setw(7)<<"20-29"<<std::setw(7)<<"30-39"<<std::setw(7)<<"40-49"<<std::setw(7)<<"50-59"
      <<std::setw(7)<<"60-69"<<std::setw(7)<<"70-79"<<std::setw(7)<<"80-89"<<std::setw(7)<<"90-99"<<std::endl<<std::endl;
      for(int i = 0; i < 10; i++){
        std::cout<<std::setw(7)<<cs[i];
      }
}

// on printout the histg 
void* stat_thread_func(void* args) {
      // while loop to pick up data from stat PCB
      // till the done from work , break
      // print histg
  STFArgs* stfa = (STFArgs*) args;
  while(wNotDone){
    if(stfa->patient_name == "joe" && stfa->PCB->length() > 0) {
      joe_q.push(stfa->PCB->Retrieve());
    }else if(stfa->patient_name == "jane" && stfa->PCB->length() > 0) {
      jane_q.push(jane_b->Retrieve());
    }else if(stfa->patient_name == "john" && stfa->PCB->length() > 0) {
      john_q.push(john_b->Retrieve());
    }
  }
  return stfa;
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

/* client 
  -n <number of data requests per patient>
  -b <size of bounded buffer between request and worker threads>
  -w <number of worker threads>
*/

int main(int argc, char * argv[]) {
  timeval start, end;
  gettimeofday(&start, nullptr); // start timer
  int option = 0;
  while((option = getopt(argc, argv, "n:b:w:"))!= -1) {
    switch(option) {
      case 'n' :
        n_req = atoi(optarg);
        break;
      case 'b' :
        b_size = atoi(optarg);
        break;
      case 'w':
        n_work = atoi(optarg);
        break;
      case '?':
        std::cout<<"invalid command line argument"<<std::endl;
        assert(false);
      default :
        std::cout<<"default"<<std::endl;
        assert(false);
    }
  }
  // initializations
  PCBuffer<std::string>* PCB= new PCBuffer<std::string>(b_size);
  pthread_t w_threads[n_work];
  pthread_t r_threads[3];
  joe_b = new PCBuffer<std::string>(b_size);
  jane_b = new PCBuffer<std::string>(b_size);
  john_b = new PCBuffer<std::string>(b_size);

  pid_t pid;
  if ((pid = fork()) == 0) {
    // child process for server
    system("./dataserver");
  }else {
    std::cout << "CLIENT STARTED:" << std::endl;
    std::cout << "Establishing control channel... " << std::flush;
    RequestChannel chan("control", RequestChannel::Side::CLIENT);
    std::cout << "done." << std::endl;
    
    std::cout<<"start to create request threads"<<std::endl;
    RTFArgs* Rjoe = new RTFArgs();
    Rjoe->n_req = n_req;
    Rjoe->patient_name = "joe";
    Rjoe->PCB = PCB;
    pthread_create(&r_threads[0], nullptr, request_thread_func, (void*)Rjoe);
    RTFArgs* Rjane = new RTFArgs();
    Rjane->n_req = n_req;
    Rjane->patient_name = "jane";
    Rjane->PCB = PCB;
    pthread_create(&r_threads[1], nullptr, request_thread_func, (void*)Rjane);
    RTFArgs* Rjohn = new RTFArgs();
    Rjohn->n_req = n_req;
    Rjohn->patient_name = "john";
    Rjohn->PCB = PCB;
    pthread_create(&r_threads[2], nullptr, request_thread_func, (void*)Rjohn);

    // std::cout<<"start to create worker threads"<<std::endl;
    // std::cout<<"number of work thread is "<<n_work<<std::endl;
    for(int i = 0; i < n_work; i++) {
      std::string reply = chan.send_request("newthread"); 
      RequestChannel* channel = new RequestChannel(reply,RequestChannel::Side::CLIENT); // create one channel for each w_thread
      // std::cout<<"Reply to request 'newthread' is "<< reply.c_str()<<std::endl;
      WTFArgs* wtfa = new WTFArgs();
      wtfa->PCB = PCB;
      wtfa->rc = channel; // assign channels to each w_thread
      pthread_create(&w_threads[i], nullptr, worker_thread_func, (void*)wtfa);
    }

    // std::cout << "starting to create stat threads"<<std::endl;
    STFArgs* Sjoe = new STFArgs();
    Sjoe->patient_name = "joe";
    Sjoe->PCB = joe_b;
    pthread_create(&s_threads[0], nullptr, stat_thread_func, (void*)Sjoe);
    STFArgs* Sjane = new STFArgs();
    Sjane->patient_name = "jane";
    Sjane->PCB = jane_b;
    pthread_create(&s_threads[1], nullptr, stat_thread_func, (void*)Sjane);
    STFArgs* Sjohn = new STFArgs();
    Sjohn->patient_name = "john";
    Sjohn->PCB = john_b;
    pthread_create(&s_threads[2], nullptr, stat_thread_func, (void*)Sjohn);
    
    pthread_join(r_threads[0],nullptr);
    pthread_join(r_threads[1],nullptr);
    pthread_join(r_threads[2],nullptr); // wait for all r_threads are done
    // to indicate r_threads all are done 
    for(int i = 0; i < n_work; i++) {
      PCB->Deposit("quit"); // quit all workers
    }
    for(int i = 0; i < n_work; i++) {
      pthread_join(w_threads[i],nullptr);
    }
    wNotDone = false;
    // send done to stat after work done, since stat is a forever loop 
    pthread_join(s_threads[0],nullptr);
    pthread_join(s_threads[1],nullptr);
    pthread_join(s_threads[2],nullptr); 
    std::cout<<"\n    joe"<<std::endl;printH(joe_q);
    std::cout<<"\n    jane"<<std::endl;printH(jane_q);
    std::cout<<"\n    john"<<std::endl;printH(john_q);
    std::cout<<std::endl;
    // std::cout << "finally done" << std::endl;
    // to explicitly quit server
    chan.send_request("quit");
    gettimeofday(&end, nullptr); // end timer
    std::cout<<"Time from start to end of the client is "<<end.tv_sec-start.tv_sec<<" secs and "<<end.tv_usec-start.tv_usec<<" musecs."<<std::endl;
    usleep(1000000);
  }
}
