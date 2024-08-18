
CC = g++
CL = ld
CFLAGS = -Wall -Wextra -D CL_TARGET_OPENCL_VERSION=100 -fPIC -std=c++17

SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LDFLAGS = -L. -L../OpenCL -shared
LDLIBS = -lgpu
INC = -I../OpenCL

LIBRARY = libNeuralNetwork.so

$(LIBRARY) : $(OBJECTS)
	$(CL) -verbose $(LDFLAGS) -o $(LIBRARY) $(LDLIBS) $(OBJECTS)

$(OBJECTS) : %.o : %.cpp
	$(CC) $(INC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *o $(LIBRARY)

