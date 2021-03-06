DEBUG = 1
DIR_BIN = bin
DIR_OBJ = obj
DIR_SRC = src
BIN = $(DIR_BIN)/ocr
SRC = $(wildcard $(DIR_SRC)/*.c)
OBJ = $(subst $(DIR_SRC),$(DIR_OBJ), $(SRC:.c=.o))
DEP = $(OBJ:.o=.d)

ifeq ($(DEBUG),1)
	# TODO
	# SANITIZE = -fsanitize=address
endif

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -std=c99 $(SANITIZE)
LDLIBS = -lm -lSDL2 -lSDL2_ttf
LDFLAGS = $(SANITIZE)

ifeq ($(DEBUG),1)
	CFLAGS += -g
endif

# Windows specific flags
ifeq ($(OS), Windows_NT)
	CFLAGS += -Ilib/include
	LDFLAGS += -Llib/lib -lSDL2_ttf -lSDL2
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

# Generates only the dataset
# !!! No depedencies for this rule
.PHONY: dataset
dataset:
	cd data && ./generate.sh

.PHONY: clean
clean:
	rm -rf $(DIR_BIN) $(DIR_OBJ) data/dataset_{tex,pdf,bmp}

# Make temporary directories
.PHONY: dirs
dirs:
	mkdir -p $(DIR_BIN) $(DIR_OBJ)

-include $(DEP)
