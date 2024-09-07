
CC = g++
CFLAGS = -g -Wall -Wextra

SOURCES = $(wildcard *.cpp)
#SOURCES := $(filter-out SocketConnection.cpp, $(SOURCES))
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LDFLAGS = -L. -L../OpenCL -L../NeuralNetwork  -L/home/Kosmas/OpenCL-SDK-main/install/lib/
LDLIBS = -l:libgpu.o -lOpenCL -l:libNeuralNetwork.o -lraylib -lGL -lm -lpthread -ldl -lrt
INC = -I../OpenCL -I../NeuralNetwork -I/usr/include/

EXECUTABLE = Chess

$(EXECUTABLE) : $(OBJECTS)
	$(CC) --verbose  $(CLFALS) $(LDFLAGS) $(LDLIBS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS) : %.o : %.cpp
	$(CC) $(INC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *o $(EXECUTABLE)

