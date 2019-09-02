CC = g++
CC_FLAGS = -Wall -g

EXEC = main
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst src/%,build/%,$(SOURCES:.cpp=.o))
INCLUDE = -I include
LIBS = -pthread

$(EXEC): $(OBJECTS)
	$(CC) $(CC_FLAGS) $(OBJECTS) -o $(EXEC) $(LIBS) 

build/%.o: src/%.cpp
	$(CC) -c $(INCLUDE) $(CC_FLAGS) $< -o $@

clean:
	-rm -rf $(EXEC) $(OBJECTS)
