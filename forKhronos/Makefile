CC=gcc

CFLAGS   := -Iinclude -I/opt/KAYA_Instruments/include -pipe -pedantic -g -Wall -Wextra
LDFLAGS  := -L/opt/KAYA_Instruments/lib -Wl,-rpath,/opt/KAYA_Instruments/lib -Wl,-rpath,'$ORIGIN'  
LDLIBS   := -lKYFGLib -lz -lm -ldl -lglfw

SRC_DIR:=src
BIN_DIR:=bin
OBJ_DIR:=bin/obj

EXE:=$(BIN_DIR)/Vegvisir
SRC:=$(wildcard $(SRC_DIR)/*.c)
OBJ:=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))


.PHONY: all

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@
.PHONY: clean
clean:
	rm  -r bin/*
.PHONY: start
start:
	./bin/Vegvisir