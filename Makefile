CC = gcc
BASICOPTS = -g
CFLAGS = -O2 $(BASICOPTS)
OFLAGS = -c -Wall -Wextra -D_LARGEFILE64_SOURCE
LIBS = -lm

OBJECTSDIR=obj
TARGETDIR=bin

OBJECTS = $(OBJECTSDIR)/network.o

all: clean $(TARGETDIR)/server $(TARGETDIR)/client

$(TARGETDIR)/server: $(TARGETDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ server.c $(OBJECTS) -pthread

$(TARGETDIR)/client: $(TARGETDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ client.c $(OBJECTS)

$(OBJECTSDIR)/network.o: $(OBJECTSDIR)
	$(CC) $(OFLAGS) -o $@ network.c $(LIBS)

clean:
	rm -f \
		./$(TARGETDIR)/server \
		./$(TARGETDIR)/client

$(TARGETDIR):
	mkdir -p $(TARGETDIR)

$(OBJECTSDIR):
	mkdir -p $(OBJECTSDIR)
