CC = gcc
BASICOPTS = -g
CFLAGS = -O2 $(BASICOPTS)
OFLAGS = -c -Wall -Wextra
LIBS = -lm

OBJECTSDIR=obj
TARGETDIR=bin

OBJECTS = $(OBJECTSDIR)/functions.o

all: clean $(TARGETDIR)/server $(TARGETDIR)/client

$(TARGETDIR)/server: $(TARGETDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ server.c $(OBJECTS) -pthread

$(TARGETDIR)/client: $(TARGETDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ client.c $(OBJECTS)

$(OBJECTSDIR)/functions.o: $(OBJECTSDIR)
	$(CC) $(OFLAGS) -o $@ functions.c $(LIBS)

clean:
	rm -f \
		./$(TARGETDIR)/server \
		./$(TARGETDIR)/client \
		./$(OBJECTSDIR)/functions.o

$(TARGETDIR):
	mkdir -p $(TARGETDIR)

$(OBJECTSDIR):
	mkdir -p $(OBJECTSDIR)
