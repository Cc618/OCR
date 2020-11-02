DIR_BIN = bin
DIR_OBJ = obj
DIR_SRC = src
BIN = $(DIR_BIN)/ocr
SRC = $(wildcard $(DIR_SRC)/*.c)
OBJ = $(subst $(DIR_SRC),$(DIR_OBJ), $(SRC:.c=.o))
DEP = $(OBJ:.o=.d)

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -std=c99
LDLIBS = -lm -lSDL2
LDFLAGS =

# Windows specific flags
ifeq ($(OS), Windows_NT)
	CFLAGS += -Ilib/include
	LDFLAGS += -Llib/lib
endif

all: dirs $(BIN)

.PHONY: run
run: all
	@echo --- Running ocr ---
	./$(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -rf $(DIR_BIN) $(DIR_OBJ) data/dataset_{tex,pdf,bmp}

# Make temporary directories
.PHONY: dirs
dirs:
	mkdir -p $(DIR_BIN) $(DIR_OBJ)

-include $(DEP)
