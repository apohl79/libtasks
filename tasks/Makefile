TARGET = libtasks.a
CXX = g++-4.8
CC = $(CXX)
#CXXFLAGS = -O3 -ggdb -I. -std=c++11 -pthread -DDEBUG -DLOGMUTEX
CXXFLAGS = -O4 -ggdb -I.. -std=c++11 -pthread
LDFLAGS = -lev

SRC = $(wildcard *.cpp) $(wildcard net/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	ar -rcs $@ $(OBJ)

$(OBJ): %.o: %.cpp

$(OBJ): %.o: %.h

clean:
	rm $(OBJ) $(TARGET)
