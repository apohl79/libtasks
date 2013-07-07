TARGET = libtasks.a
CXX = g++-4.8
CC = $(CXX)
DEBUG = 0

CXXFLAGS = -O4 -ggdb -I.. -std=c++11 -pthread
LDFLAGS = -lev

ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -DLOGMUTEX
endif

SRC = $(wildcard *.cpp) $(wildcard net/*.cpp tools/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	ar -rcs $@ $(OBJ)

$(OBJ): %.o: %.cpp

$(OBJ): %.o: %.h

clean:
	rm $(OBJ) $(TARGET)
