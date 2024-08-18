
CC = g++
CFLAGS = -g -Wall -Wextra -D CL_TARGET_OPENCL_VERSION=100 -std=c99

SOURCES = $(wildcard *.cpp)
#SOURCES := $(filter-out SocketConnection.cpp, $(SOURCES))
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LDFLAGS = -L. -L../OpenCL -L../NeuralNetwork  -L/usr/lib/
LDLIBS = -lgpu -lNeuralNetwork -lraylib -lGL -lm -lpthread -ldl -lrt
INC = -I../OpenCL -I../NeuralNetwork -I/usr/include/

EXECUTABLE = Chess

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(LDLIBS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS) : %.o : %.cpp
	$(CC) $(INC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *o $(EXECUTABLE)

