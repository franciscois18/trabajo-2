CC=gcc 
EXEC=program.out
EXECDB=programdb.out
GRUPO=T2
NTAR=2

SRC_DIR=src
SRCDB_DIR=srcdb
OBJ_DIR=obj
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
SRCDB_FILES=$(wildcard $(SRCDB_DIR)/*.c)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
OBJDB_FILES=$(patsubst $(SRCDB_DIR)/%.c,$(OBJ_DIR)/%_srcdb.o,$(SRCDB_FILES))
INCLUDE=-I./incs/
LIBS=
LIBS= -lm

CFLAGS=-Wall -Wextra -Wpedantic -O3
LDFLAGS= -Wall -lm

all: $(OBJDB_FILES) $(OBJ_FILES)
	$(CC) $(CFLAGS) -o build/$(EXEC) $(OBJ_FILES) $(INCLUDE) $(LIBS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(INCLUDE)

$(OBJ_DIR)/%_srcdb.o: $(SRCDB_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(INCLUDE)

build/$(EXECDB): $(OBJDB_FILES)
	$(CC) $(CFLAGS) -o build/$(EXECDB) $(OBJDB_FILES) $(INCLUDE) $(LIBS) $(LDFLAGS)

.PHONY: clean folders send

clean:
	rm -f $(OBJ_FILES) $(OBJDB_FILES)
	rm -f build/$(EXEC) build/$(EXECDB)

folders:
	mkdir -p src obj incs build docs results

send:
	tar czf $(GRUPO)-$(NTAR).tgz --transform 's,^,$(GRUPO)-$(NTAR)/,' Makefile src srcdb incs docs

run: build/$(EXEC)
	./build/$(EXEC)

run-db: build/$(EXECDB)
	./build/$(EXECDB)
