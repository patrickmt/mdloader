OUTNAME = mdloader
OBJDIR = build
CC = gcc
CFLAGS = -Wall -Os -std=gnu99

SRCFILES = mdloader_common.c
ifeq ($(OS),Windows_NT)
SRCFILES += mdloader_win32.c
else
SRCFILES += mdloader_unix.c
endif

OBJFILES = $(patsubst %.c,%.o,$(SRCFILES))
OBJS = $(addprefix $(OBJDIR)/,$(OBJFILES))

all: $(OBJDIR)/$(OUTNAME)
	$(info Copying applets to ${OBJDIR}...)
	@cp applet-*.bin $(OBJDIR)
	$(info Done!)

$(OBJDIR)/$(OUTNAME): $(OBJS)
	$(info Creating $@...)
	@$(CC) $(CFLAGS) $(OBJS) -o $@
	@rm -f $(OBJDIR)/*.o

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	@mkdir $(OBJDIR)

$(OBJS): $(OBJDIR)/%.o : %.c
	$(info Compiling ${<}...)
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(info Removing $(OBJDIR)...)
	@rm -r -f $(OBJDIR)
	$(info Done!)
