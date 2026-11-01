CC = gcc
CFLAGS = -Iinclude -MMD -MP -DPORT=3000 -DDEBUG -std=gnu99

SRC = main.c $(wildcard include/*.c)

OBJ = $(patsubst %.c,obj/%.o,$(SRC))

DEP = $(OBJ:.o=.d)

TARGET = server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

obj/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

clean:
	rm -rf obj $(TARGET)

.PHONY: all clean
