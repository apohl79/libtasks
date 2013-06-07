TARGET = echoserver
CXX = g++-4.8
CC = $(CXX)
#CXXFLAGS = -O3 -ggdb -I.. -I. -std=c++11 -pthread -DDEBUG -DLOGMUTEX
CXXFLAGS = -O3 -ggdb -I.. -I. -std=c++11 -pthread
LDFLAGS = -lev -L../tasks -ltasks

SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o $@

$(OBJ): %.o: %.cpp

$(OBJ): %.o: %.h

clean:
	rm $(OBJ) $(TARGET)
