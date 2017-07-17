# Custom_Neural_Network

(No pictures, as there is no real way to illustrate this)

This is a basic Artificial Neural Network implemented in C++, without a framework. 
However, this implementation was made for a very specific task (prediction of vectors extracted from Gabor filters in a video) and is unfit for basically anything else. 
Moreover, as this program was designed to run on a Cluster of computer, the tasks are managed by as many threads as possible. For speed reasons, nearly all the loops in the NeuralNet class were done using chained lists, and are therefore hard to read. 


Features included : 
-Propagation of the input(s) of the network.
-Computation of the output layer error gradient.
-Backpropagation of this gradient to the hidden layer(s).
-Modification of the weights of the network according to the propagated gradient.

Compilation & run : 

cd Sources
make
./DNN.out

The network will then create as much threads as possible, grab the databases specified in the Jobs/JOBS.txt file (only 7 were included in this repository), train a few thousand networks using them, and output the performances in the Results directory. 
