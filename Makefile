CC = gcc
CFLAGS = -Iinclude -Iinclude/parsers -MMD -MP -std=gnu99 -O3
SRC = main.c $(wildcard include/*.c) $(wildcard include/parsers/*.c) 

ifdef ENDPOINTS
	CFLAGS += -DENDPOINTSFOLDER="$(ENDPOINTS)" -I"$(ENDPOINTS)"
	SRC += $(wildcard $(ENDPOINTS)/*.c)
endif

ifdef PORT
	CFLAGS += -DPORT=$(PORT) 
else
	CFLAGS += -DPORT=80 
endif

ifdef DEBUG
	CFLAGS += -DDEBUG -g
endif

ifdef HASHSIZE
	CFLAGS += -DENDPOINTHASHSIZE=$(HASHSIZE)
endif

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
