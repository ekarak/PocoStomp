#
# Makefile for Thrift4OZW
# Elias Karakoulakis <elias.karakoulakis@gmail.com>
# based on Makefile for OpenWave Control Panel application by Greg Satz

# GNU make only

.SUFFIXES:	.cpp .o .a .s

CC     := gcc
CXX    := g++
LD     := g++
AR     := ar rc
RANLIB := ranlib

# Change for DEBUG or RELEASE
TARGET := DEBUG

DEBUG_CFLAGS    := -Wall -Wno-format -g -DDEBUG -Werror -O0 -DDEBUG_POCOSTOMP
RELEASE_CFLAGS  := -Wall -Wno-unknown-pragmas -Wno-format -O3 -DNDEBUG

DEBUG_LDFLAGS	:= -g

# change  SMC install directory if needed
SMC := /opt/smc

CFLAGS	:= -c $($(TARGET)_CFLAGS) 
LDFLAGS	:= $($(TARGET)_LDFLAGS) -L/usr/lib/ -L/usr/local/lib

INCLUDES := -I $(SMC)/lib/C++/ -I .

# Poco libs are dynamic only (at least in Debian)
LIBS := -lPocoNet -lPocoFoundation 

%.o : %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

all: main
    
Stomp_sm.cpp: Stomp.sm
	smc -c++ Stomp.sm 

Stomp_sm.o: Stomp_sm.cpp 
	g++ $(CFLAGS) -c Stomp_sm.cpp $(INCLUDES)
    
StompSocket.o: StompSocket.cpp StompSocket.h
	g++ $(CFLAGS) -c StompSocket.cpp $(INCLUDES)
    
PocoStomp.o:  PocoStomp.cpp PocoStomp.h Stomp_sm.cpp StompSocket.o
	g++ $(CFLAGS) -c PocoStomp.cpp $(INCLUDES)    

Main.o: Main.cpp Stomp_sm.o
	g++ $(CFLAGS) -c Main.cpp $(INCLUDES)  
	
main:   Main.o  Stomp_sm.o StompSocket.o PocoStomp.o 
	$(LD) -o $@ $(LDFLAGS) Main.o Stomp_sm.o StompSocket.o PocoStomp.o $(LIBS)

dist:	main
	rm -f Thrift4OZW.tar.gz
	tar -c --exclude=".git" --exclude ".svn" --exclude "*.o" -hvzf Thrift4OZW.tar.gz *.cpp *.h *.thrift *.sm *.rb Makefile gen-*/ license/ README*

bindist: main
	rm -f Thrift4OZW_bin_`uname -i`.tar.gz
	upx ozwd*
	tar -c --exclude=".git" --exclude ".svn" -hvzf Thrift4OZW_bin_`uname -i`.tar.gz ozwd license/ README*

clean:
	rm -f ozwd*.o Stomp_sm.* gen-cpp/RemoteManager.cpp gen-cpp/RemoteManager_server.cpp gen-cpp/ozw_types.h

binclean: 
	rm -f ozwd *.o 