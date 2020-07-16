CFLAGS = -Wall -g -std=c11
BIN = bin/
INC = include/
SRC = src/
XML_PATH = /usr/include/libxml2


# Standard common makefile /// 31350   http://cis2750.socs.uoguelph.ca:31350/ ////// to compile the test file use -> gcc src/test.c -Lbin/libsvgparse.so -I/usr/include/libxml2 -Iinclude/ -lxml2
parser: $(BIN)libsvgparse.so

test: parser
	gcc -o test src/test.c -Lbin/ -Iinclude/ -lsvgparse -I/usr/include/libxml2 -lxml2 -lm 

$(BIN)libsvgparse.so: $(BIN)SVGParser.o $(BIN)LinkedListAPI.o
	gcc -shared -o $(BIN)libsvgparse.so $(BIN)SVGParser.o $(BIN)LinkedListAPI.o -lxml2 -lm

$(BIN)SVGParser.o: $(SRC)SVGParser.c $(INC)LinkedListAPI.h $(INC)SVGParser.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGParser.c -o $(BIN)SVGParser.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	gcc -c -fpic $(CFLAGS) -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

clean:
	rm $(BIN)*.o $(BIN)*.so
