TARGET = thrift_client
CXX = g++-4.8
CC = $(CXX)
DEBUG = 0
PROFILER = 0

CXXFLAGS = -O4 -ggdb -I../.. -I. -Igen-cpp -I/usr/include/thrift -I/usr/local/include/thrift -std=c++11 -pthread
LDFLAGS = -L../../tasks -ltasks -lev -lthrift

ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -DLOGMUTEX
endif

ifeq ($(PROFILER), 1)
  CXXFLAGS += -DPROFILER
  LDFLAGS += -lprofiler
endif

SRC = $(wildcard *.cpp) $(wildcard gen-cpp/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))

all: gen-cpp $(TARGET)

gen-cpp: test.thrift
	thrift --gen cpp test.thrift
	rm gen-cpp/test_service_server.skeleton.cpp
	$(MAKE) $(MAKE_ARGS)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o $@

$(OBJ): %.o: %.cpp

#$(OBJ): %.o: %.h

clean:
	rm $(OBJ) $(TARGET)
	rm -r gen-cpp
