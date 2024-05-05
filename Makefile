CC = cc
CFLAGS += -Wall -Wextra
CFLAGS += $(CPPFLAGS)

SOURCES = main.c neco/neco.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = echo

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
