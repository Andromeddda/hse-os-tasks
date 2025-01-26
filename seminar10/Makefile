CC = gcc

FLAGS = -Wall -Werror -Wextra -g -std=c99 -O0

SRC = src
BUILD = build

SOURCE_FILES = $(notdir $(wildcard $(SRC)/*.c))
EXE_FILES = $(SOURCE_FILES:%.c=$(BUILD)/%)

all: $(EXE_FILES)

$(BUILD)/% : $(SRC)/%.c Makefile
	@mkdir -p $(BUILD)
	$(CC) -o $@ $(FLAGS) $<

clean:
	rm -f $(BUILD)/*

.PHONY : all, clean