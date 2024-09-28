CC = g++
CFLAGS = -g -Wall -Wextra


ifdef main
$(eval MAIN= $(main))
else
$(eval MAIN= main.cpp)
endif


SOURCES = $(wildcard *.cpp)
SOURCES := $(filter-out main.cpp, $(SOURCES))
SOURCES += $(MAIN)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
OBJECTS := $(filter-out main.cpp main.o, $(OBJECTS))
OBJECTS += $(MAIN)

LDFLAGS = -L. -L/home/Kosmas/OpenCL-SDK-main/install/lib/ -L../NeuralNetwork -L../OpenCL
LDLIBS =  -lOpenCL -lraylib -lGL -lm -lpthread -ldl -lrt -l:libNeuralNetwork.o -l:libgpu.o
INC = -I../OpenCL -I../NeuralNetwork -I/usr/include/ -I.. -I.

EXECUTABLE = Chess




$(EXECUTABLE) : $(OBJECTS) ../OpenCL/libgpu.o ../NeuralNetwork/libNeuralNetwork.o
ifneq ($(MAIN), main.cpp)
	echo "Main defined"
	$(CC)  $(CFLAGS) $(LDFLAGS) $(INC) $(LDLIBS) -o $(EXECUTABLE) $(filter-out $(main), $(OBJECTS))
else
	echo "Main not defined"
	$(CC)  $(CFLAGS) $(LDFLAGS) $(INC) $(LDLIBS) -o $(EXECUTABLE) $(OBJECTS)
endif

$(OBJECTS) : %.o : %.cpp
	$(CC) $(INC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *o $(EXECUTABLE)
