CC = gcc
BASICOPTS = -g -D_LARGEFILE64_SOURCE
CFLAGS = -O2 $(BASICOPTS)
OFLAGS = -c -Wall -Wextra
LIBS = -lm

OBJECTSDIR=obj
TARGETDIR=bin

OBJECTS = 

all: clean $(TARGETDIR)/server $(TARGETDIR)/client $(TARGETDIR)/test

$(TARGETDIR)/server: $(TARGETDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ server.c $(OBJECTS) -pthread

$(TARGETDIR)/client: $(TARGETDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ client.c $(OBJECTS)

$(TARGETDIR)/test: $(TARGETDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ test.c $(OBJECTS)

#$(OBJECTSDIR)/functions.o: $(OBJECTSDIR)
#	$(CC) $(OFLAGS) -o $@ functions.c $(LIBS)

clean:
	rm -f \
		./$(TARGETDIR)/server \
		./$(TARGETDIR)/client

$(TARGETDIR):
	mkdir -p $(TARGETDIR)

$(OBJECTSDIR):
	mkdir -p $(OBJECTSDIR)
