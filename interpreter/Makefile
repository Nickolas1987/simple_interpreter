CC=g++
CXXFLAGS=-c -pthread -std=c++1y -Wall -fPIC  -pthread
INC=-I./include
SOURCES= ./src/main.cpp ./src/log.cpp ./src/interpreter.cpp ./src/syntaxer.cpp ./src/lexer.cpp ./src/token.cpp ./src/token_value.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=simple_interpreter

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ -pthread
.cpp.o:
	$(CC) $(CXXFLAGS) $< -o $@ $(INC) -pthread
clean:
	rm -rf ./src/*.o simple_interpreter
