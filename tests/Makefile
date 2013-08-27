TARGET = libtasks_tests
CXX = g++-4.8
CC = $(CXX)

CXXFLAGS = -ggdb -I../.. -I.. -I. -I/usr/include/thrift -std=c++11 -pthread
LDFLAGS = -L../../tasks -L../tasks -ltasks -lev -lcppunit

SRC = $(wildcard *.cpp) $(wildcard gen-cpp/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))

all: $(TARGET) tests

tests: $(TARGET) nginx.pid
	./libtasks_tests
	@nginx -p . -c nginx.conf -s stop

nginx.pid:
	@nginx -p . -c nginx.conf


$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o $@

$(OBJ): %.o: %.cpp

clean:
	rm $(OBJ) $(TARGET)
