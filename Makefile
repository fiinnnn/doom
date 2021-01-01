CC=gcc
CFLAGS=-Wall `sdl2-config --cflags`
LDFLAGS=
LDLIBS= `sdl2-config --libs`

BINDIR=bin
OBJDIR=obj

OBJS=\
	$(OBJDIR)/log.o\
	$(OBJDIR)/i_video.o\
	$(OBJDIR)/c_video.o\
	$(OBJDIR)/c_mem.o\
	$(OBJDIR)/w_wad.o\
	$(OBJDIR)/p_level.o\
	$(OBJDIR)/r_main.o\
	$(OBJDIR)/am_map.o\

.PHONY: debug
debug: CFLAGS+= -g -DDEBUG
debug: $(BINDIR)/doom

.PHONY: release
release: CFLAGS+= -O3
release: $(BINDIR)/doom

.PHONY: clean
clean:
	rm -rf obj/
	rm -rf bin/

$(BINDIR)/doom: $(OBJS) $(OBJDIR)/main.o | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(OBJDIR)/main.o \
	-o $(BINDIR)/doom $(LDLIBS)

$(OBJDIR)/%.o: %.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p $(OBJDIR)

bin:
	mkdir -p $(BINDIR)
