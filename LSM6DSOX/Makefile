DEBUG= -O2 #Debugging Level

CC= gcc #Compiler command
INCLUDE= -I/usr/local/include #Include directory
CFLAGS= $(DEBUG) -Wall $(INCLUDE) -Winline #Compiler Flags
LDFLAGS= -L/usr/local/lib #Linker Flags

LIBS= -lpthread -lm #Libraries used if needed

SRC = main.cpp i2c_master.cpp

OBJ = $(SRC:.cpp=.o)
BIN = $(SRC:.cpp=)

$(BIN): $(OBJ)
	@echo [link] $@
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)
.cpp.o:
	@echo [Compile] $<
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	@rm -f $(OBJ) $(BIN)
