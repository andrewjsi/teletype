CC = gcc
CFLAGS = -Wall
LIBS = -lutil
OBJ=stringconv.o teletype.o
BIN=teletype

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LIBS)

doc:
	doxygen
	@echo ""
	@echo "DOC URL: file://"`pwd`"/html/index.html"
	@echo ""

clean:
	rm -rf $(OBJ) $(BIN) dump core html/ latex/ man/

# Függőségek
#policy.o: server.h
