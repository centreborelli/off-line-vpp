CFLAGS=-Wall -Wextra  -O4 #-Werror
LFLAGS=-lpng -ljpeg -ltiff -fopenmp 


# Recursively get all *.cpp and *.c in this directory and any sub-directories
SRC1 := $(shell find . -name "*.cpp") 
SRC2 := $(shell find . -name "*.c") 

INCLUDE = -I.

#Replace suffix .cpp and .c by .o
OBJ := $(addsuffix .o,$(basename $(SRC1))) $(addsuffix .o,$(basename $(SRC2)))

#Binary file
BIN = orsa

#All is the target (you would run make all from the command line). 'all' is dependent
all: $(BIN)

#$(BIN) is dependent on objects
$(BIN): $(OBJ)
	g++ $(OBJ)  -o $@ $(LFLAGS) -lstdc++

#each object file is dependent on its source file, and whenever make needs to create
#an object file, to follow this rule:
%.o: %.c
	gcc -std=c99  -c $< -o $@ $(INCLUDE) $(CFLAGS) $(LFLAGS) 

%.o: %.cpp
	g++ -c $< -o $@ $(INCLUDE) $(CFLAGS) $(LFLAGS) 

clean: 
	rm -f $(OBJ) $(BIN)
