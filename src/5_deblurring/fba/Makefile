CC=c99

OBJ=main.o fba.o consistent_registration.o utils.o
EXT_OBJ=iio.o
EXT_OBJ+=gaussian_20131215/basic.o gaussian_20131215/filter_util.o gaussian_20131215/invert_matrix.o gaussian_20131215/gaussian_short_conv.o gaussian_20131215/gaussian_conv_vyv.o
EXT_OBJ+=tvl1flow_3/tvl1flow_lib.o

OPTIMISATIONS=-O3 -march=native -fopenmp
CFLAGS+=-Igaussian_20131215/ ${OPTIMISATIONS}
LDFLAGS+=-lm -lpng -ltiff -ljpeg -lfftw3 ${OPTIMISATIONS}

all: fba

fba: ${OBJ} ${EXT_OBJ}
	${CC} $^ -o $@ ${CCFLAGS} ${LDFLAGS}

clean:
	rm -f ${OBJ} ${EXT_OBJ} fba
