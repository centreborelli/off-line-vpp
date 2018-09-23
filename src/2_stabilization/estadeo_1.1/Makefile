CFLAGS=-Wall -Wextra -O3 #-Werror   
LFLAGS=-lstdc++ -fopenmp -lm -lfftw3 -lfftw3f -lpng -ljpeg -ltiff 

# Recursively get all *.cpp and *.c in this directory and any sub-directories
SRC1 := $(shell find ./src -name "*.cpp") 
SRC1 := $(filter-out ./src/generate_graphics.cpp, $(SRC1))
SRC2 := $(shell find ./src -name "*.c") 

INCLUDE = -I./src -I./src/feature_based_method

#Replace suffix .cpp and .c by .o
OBJ := $(addsuffix .o,$(basename $(SRC1))) $(addsuffix .o,$(basename $(SRC2)))
OBJ := $(filter-out ./src/generate_graphics.o, $(OBJ))
OBJ := $(filter-out ./src/feature_based_method/libImage/image_io.o, $(OBJ))
#OBJ := $(filter-out ./src/direct_method/iio.o, $(OBJ))

#executable files
all: bin/estadeo bin/generate_graphics

#generate executables
bin/estadeo: $(OBJ) 
	g++ -std=c++11 $^ -o $@  $(LFLAGS)

bin/generate_graphics: ./src/generate_graphics.o ./src/gaussian_conv_dct.o  
	g++ -std=c++11 ./src/gaussian_conv_dct.o $< -o $@ $(CFLAGS) $(LFLAGS) 

#compile object files
%.o: %.c
	gcc -std=c99 -c $< -o $@ $(INCLUDE) $(CFLAGS) $(LFLAGS) -Wno-unused -pedantic -DNDEBUG -D_GNU_SOURCE

%.o: %.cpp
	g++ -std=c++11 -c $< -o $@ $(INCLUDE) $(CFLAGS) $(LFLAGS)

clean: 
	rm -f $(OBJ) bin/estadeo bin/estadeo_only_smoothing bin/generate_graphics src/generate_graphics.o

