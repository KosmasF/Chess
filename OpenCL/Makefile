CC = g++
CL = ld
CFLAGS = -g -fPIC

SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LDFLAGS = -relocatable
INC = -I/usr/include

LIBRARY = libgpu.o

$(LIBRARY) : $(OBJECTS)
	$(CL) --verbose $(LDFLAGS) -o  $(LIBRARY) $(OBJECTS)

$(OBJECTS) : %.o : %.cpp	
	$(CC) $(INC) $(CFLAGS)  -c $^ -o $@

clean:
	rm -f *o  $(LIBRARY)

