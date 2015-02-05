CFLAGS=$(shell pkg-config --cflags --libs libftdi1) -g -Wall
DEST=ftdi_cbus_config
SRC=${DEST}.c

all: Makefile $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(DEST) 

clean:
	@rm -rf $(DEST) *.o
