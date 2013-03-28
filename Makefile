SAHN_VERSION = b6a2c3b

CC = gcc
CFLAGS = -c -Isrc -Ilib -pthread
debug : CFLAGS += -g -O0

LL = gcc
LFLAGS = -Wl,-rpath,lib -Llib -lsahn$(D) -lpulse-simple -pthread

OBJDIR = obj

OBJ = $(OBJDIR)/main.o

debug : D = _d


all: lib/libsahn.so $(OBJDIR) vahn node

debug: lib/libsahn_d.so $(OBJDIR) vahn node

$(OBJDIR):
	@mkdir -p $(OBJDIR)

lib/libsahn.so:
	@$(MAKE) -C lib/sahn dist
	@mv lib/sahn/libsahn-$(SAHN_VERSION)/libsahn.so lib/sahn/libsahn-$(SAHN_VERSION)/sahn.h lib

lib/libsahn_d.so:
	@$(MAKE) -C lib/sahn debug
	@cp lib/sahn/sahn/sahn.h lib
	@mv lib/sahn/bin/libsahn_d.so lib

clean:
	@$(MAKE) -C lib/sahn clean
	@rm -rf $(OBJDIR) vahn lib/sahn.h lib/libsahn.so lib/libsahn_d.so node

vahn: $(OBJ)
	$(LL) $(LFLAGS) -o vahn $(OBJ)

node: src/node.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/node.o src/node.c
	$(LL) $(LFLAGS) -o node $(OBJDIR)/node.o

$(OBJDIR)/main.o: src/main.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/main.o src/main.c
