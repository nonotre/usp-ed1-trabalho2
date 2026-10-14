
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
HEADER = ListaComCursor.h
TARGET = main

# Detecta a implementacao disponivel
SRC = main.c
ifneq ($(wildcard lista_sequencial.c),)
	SRC += lista_sequencial.c
endif
ifneq ($(wildcard lista_encadeada.c),)
	SRC += lista_encadeada.c
endif

OBJS = $(SRC:.c=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)