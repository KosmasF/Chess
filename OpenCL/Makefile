CC = g++
CFLAGS = -g -fPIC -Wall -O0

SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LDFLAGS = -shared
INC = -I/usr/include -I../NeuralNetwork

LIBRARY = libgpu.so

$(LIBRARY) : $(OBJECTS)
	$(CC) $(LDFLAGS) -o  $(LIBRARY) $(OBJECTS)

$(OBJECTS) : %.o : %.cpp	
	$(CC) $(INC) $(CFLAGS)  -c $^ -o $@

clean:
	rm -f *o  $(LIBRARY)

