TARGET=a.out
all: $(TARGET)

#SRCS=$(shell find -name '*.c')
SRCS=\
	dyn_array.c \
	song_ratings.c
OBJS=$(SRCS:%.c=%.o)

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif

INCS=
SYSINCS=

CFLAGS=-O0 -g -Wall
GST_FLAGS=$(shell pkg-config --cflags --libs gstreamer-0.10)

$(TARGET) : $(OBJS)
	gcc -o $@ $(OBJS)

%.o: %.c
	gcc $(CFLAGS)\
	$(INCS:%=-I%) $(SYSINCS:%=-isystem%) -c \
	-MMD -MP -MF"$(@:$.o=$.d)" -MT"$@" \
	-o $@ $<

clean :
	rm -rf *.o *.d *~ core

.PHONY: all clean
.SUFFIXES: