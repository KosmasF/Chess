ifdef main
$(eval MAIN= $(main))
else
$(eval MAIN= main.cpp)
endif

build:

	make -C OpenCL
	make -C NeuralNetwork
	make -C Chess main=$(MAIN)

clean:
	make -C OpenCL clean
	make -C NeuralNetwork clean
	make -C Chess clean

