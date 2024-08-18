
CC = g++
CL = ld
CFLAGS = -Wall -Wextra -D CL_TARGET_OPENCL_VERSION=100 -fPIC -pedantic

SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LDFLAGS = -L.  -L/usr/lib -shared
LDLIBS = -lOpenCL
INC = -I/usr/include

LIBRARY = libgpu.so

$(LIBRARY) : $(OBJECTS)
	$(CL) $(LDFLAGS) -o  $(LIBRARY) $(LDLIBS) $(OBJECTS)

$(OBJECTS) : %.o : %.cpp	
	$(CC) $(INC) $(CFLAGS)  -c $^ -o $@

clean:
	rm -f *o  $(LIBRARY)

