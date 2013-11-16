#
# PARMAKE - CS 241
#

CC = gcc
INC = -I.
FLAGS = -g -W -Wall
LIBS = -lpthread

SOURCES = parmake.c queue.c parser.c
OBJECTS = parmake.o queue.o parser.o


all: parmake

-include $(SOURCES:.c=.d)

parmake: $(OBJECTS)
	$(CC) $(LIBS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(INC) $(FLAGS) -c $< -o $@

%.d: %.c
	@echo "Calculating depedencies for $<"
	@ set -e; $(CC) -MM $(CFLAGS) $< \
                | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@;
	@[ -s $@ ] || rm -f $@

clean:
	$(RM) *.o parmake *.d *~
