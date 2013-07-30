TARGET = libtasks.a
CXX = g++-4.8
CC = $(CXX)
DEBUG = 0

CXXFLAGS = -O4 -ggdb -I.. -std=c++11 -pthread -DTASKS_VERSION=\"$(VERSION)\"
LDFLAGS = -lev

ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -DLOGMUTEX
endif

OS := $(shell uname)
ifeq ($(OS), Linux)
  CACHE_LINE_SIZE := $(shell getconf LEVEL1_DCACHE_LINESIZE)
endif
ifeq ($(OS), Darwin)
  CACHE_LINE_SIZE := $(shell sysctl -n hw.cachelinesize)
endif
CXXFLAGS += -DCACHE_LINE_SIZE=$(CACHE_LINE_SIZE)

SRC = $(wildcard *.cpp) $(wildcard net/*.cpp tools/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	ar -rcs $@ $(OBJ)

$(OBJ): %.o: %.cpp

$(OBJ): %.o: %.h

clean:
	rm $(OBJ) $(TARGET)
