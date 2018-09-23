WFLAGS=
CFLAGS=$(WFLAGS) -O3 -DNDEBUG 
IIOFLAGS= -ljpeg -ltiff -lpng -lm
CFLAGSIIO = $(CFLAGS) -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700

bin/convicon : src/converter.cpp src/cmd_option.h bin/iio.o
	mkdir -p bin
	g++ $(CFLAGS) -Ilib/iio/ $^ -o $@ $(IIOFLAGS)

bin/iio.o : lib/iio/iio.c lib/iio/iio.h
	mkdir -p bin
	gcc -std=c99 $(CFLAGSIIO) -c $< -o $@

clean:
	rm bin/iio.o

purge:
	rm bin/convicon
