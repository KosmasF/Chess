build:
	make -C OpenCL
	make -C NeuralNetwork
	make -C Chess

clean:
	make -C OpenCL clean
	make -C NeuralNetwork clean
	make -C Chess clean

