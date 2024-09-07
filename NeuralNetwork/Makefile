
CC = g++
CL = ld
CFLAGS = -g -fPIC

SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LDFLAGS = -relocatable
INC = -I../OpenCL

LIBRARY = libNeuralNetwork.o

$(LIBRARY) : $(OBJECTS)
	$(CL) $(LDFLAGS) -o $(LIBRARY) $(LDLIBS) $(OBJECTS)

$(OBJECTS) : %.o : %.cpp
	$(CC) $(INC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *o $(LIBRARY)

