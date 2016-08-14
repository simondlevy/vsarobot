# Makefile for behavior-based robot simulator
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as 
#    published by the Free Software Foundation, either version 3 of the 
#    License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#    
#    Simon D. Levy      Washington & Lee University       April 2013

VREP_DIR = /Applications/V-REP_PRO_EDU_V3_0_4_Mac/programming
#VREP_DIR = /home/levys/V-REP_PRO_EDU_V3_0_2_64_Linux/programming

ARCH = classical
#ARCH = vsa

#TEST = -D_TEST

ALL = bubbleRobClient

all = $(ALL)

INC = -I$(VREP_DIR)/remoteApi -I$(VREP_DIR)/include
VREP_CFLAGS = -DNON_MATLAB_PARSING
CFLAGS = -Wall
LDFLAGS = -lpthread
OBJS = $(VREP_DIR)/remoteApi/extApi.o $(VREP_DIR)/remoteApi/extApiPlatform.o
OUTPUT = ./bubbleRobClient

ifeq ($(shell uname -s), Linux)
	VREP_CFLAGS += -D__linux
else
	VREP_CFLAGS += -D__APPLE__
endif

bubbleRobClient: bubbleRobClient.o $(ARCH).o
	$(CXX) $(OBJS) bubbleRobClient.o $(ARCH).o \
	    -o bubbleRobClient $(LDFLAGS)

bubbleRobClient.o: bubbleRobClient.cpp control.hpp
	$(CXX) $(INC) $(CFLAGS) $(VREP_CFLAGS) $(TEST) -c bubbleRobClient.cpp

vsa.o: vsa.cpp control.hpp
	$(CXX) $(CFLAGS) -c vsa.cpp
	
clean:
	rm -f *.o bubbleRobClient log.csv *~

backup:
	cp *.cpp bak
	cp *.hpp bak
	cp *.ttt bak
	cp README bak
	cp Makefile bak
