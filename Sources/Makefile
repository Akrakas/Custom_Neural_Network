CC = g++

CXXFLAGS = -std=c++11 -pthread

SOURCES = Cellule_class.cpp Link_class.cpp Sequence.cpp Database_class.cpp NeuralNet.cpp function.cpp main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = $(SOURCES:.cpp=.h)
EXECUTABLE = DNN.out

all: $(SOURCES) $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CXXFLAGS) -c $< -o $@ 

clean:
	rm -f *.o DNN.out
