
CC = g++
CFLAGS = -g -Wall -Wextra

SOURCES = $(wildcard *.cpp)
#SOURCES := $(filter-out SocketConnection.cpp, $(SOURCES))
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LDFLAGS = -L. -L/home/Kosmas/OpenCL-SDK-main/install/lib/ -L../NeuralNetwork -L../OpenCL
LDLIBS =  -lOpenCL -lraylib -lGL -lm -lpthread -ldl -lrt -l:libNeuralNetwork.o -l:libgpu.o
INC = -I../OpenCL -I../NeuralNetwork -I/usr/include/

EXECUTABLE = Chess

$(EXECUTABLE) : $(OBJECTS) ../OpenCL/libgpu.o ../NeuralNetwork/libNeuralNetwork.o
	$(CC)  $(CFLALS) $(LDFLAGS) $(LDLIBS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS) : %.o : %.cpp
	$(CC) $(INC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *o $(EXECUTABLE)
