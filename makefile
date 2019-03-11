CC=g++
SOURCES=main.cpp OneWire.cpp OneWireSensor.cpp CRC.c
LIBS=-lwiringPi -lpthread

all:
        $(CC) $(SOURCES) $(LIBS) -o OneWireTest
clean:
        rm -rf *.o