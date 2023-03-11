# DEBUG= -O2 #Debugging Level

# CC= gcc #Compiler command
# INCLUDE= -I/usr/local/include #Include directory
# CFLAGS= $(DEBUG) -Wall $(INCLUDE) -Winline #Compiler Flags
# LDFLAGS= -L/usr/local/lib #Linker Flags

# LIBS= -lpthread -lm #Libraries used if needed

# SRC = BME688.cpp ./bme68x/bme68x.c

# OBJ = $(SRC:.c=.o)
# BIN = $(SRC:.c=)

# $(BIN): $(OBJ)
	# @echo [link] $@
	# $(CC) -o $@ $^ $(LDFLAGS) $(LIBS)
# .c.o:
	# @echo [Compile] $<
	# $(CC) -c $(CFLAGS) $< -o $@

# clean:
	# @rm -f $(OBJ) $(BIN)
VPATH = ./bme68x:../i2c_master

BME688.o : BME688.cpp
	g++ -c -std=c++0x -o BME688.o BME688.cpp

bme68x.o : bme68x.c 
	gcc -c -std = c99  -o ./bme68x/bme68x.o  ./bme68x/bme68x.c 

i2c_master.o : i2c_master.cpp 
	g++ -c -std=c++0x -o ../i2c_master/i2c_master.o ../i2c_master/i2c_master.cpp 

myapp : BME688.o ./bme68x/bme68x.o ../i2c_master/i2c_master.o
	g++ -o myapp BME688.o ./bme68x/bme68x.o ../i2c_master/i2c_master.o
