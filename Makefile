CFLAGS=$(shell pkg-config --cflags libftdi1) -g -Wall
LDFLAGS=$(shell pkg-config --libs libftdi1)
DEST=ftdi_cbus_config
SRC=${DEST}.c

all: Makefile $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(DEST) $(LDFLAGS)

clean:
	@rm -rf $(DEST) *.o
