# makefile

all: server client

common.o: common.h common.cpp
	g++ -g -w -std=c++11 -c common.cpp

FIFOreqchannel.o: FIFOreqchannel.h FIFOreqchannel.cpp
	g++ -g -w -std=c++11 -c FIFOreqchannel.cpp

MQreqchannel.o: MQreqchannel.h MQreqchannel.cpp
	g++ -g -w -std=c++11 -c MQreqchannel.cpp

client: client.cpp FIFOreqchannel.o MQreqchannel.o common.o
	g++ -g -w -std=c++11 -o client client.cpp FIFOreqchannel.o MQreqchannel.o common.o -lpthread

server: server.cpp  FIFOreqchannel.o MQreqchannel.o common.o
	g++ -g -w -std=c++11 -o server server.cpp FIFOreqchannel.o MQreqchannel.o common.o -lpthread

clean:
	rm -rf *.o *.csv fifo* server client data*_*
