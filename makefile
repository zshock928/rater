TARGET=a.out
all: $(TARGET)

#SRCS=$(shell find -name '*.c')
SRCS=\
	dyn_array.c \
	song_ratings.c \
	song.c \
	player.c
OBJS=$(SRCS:%.c=%.o)

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif

INCS=
SYSINCS=

GST_FLAGS=$(shell pkg-config --cflags-only-I gstreamer-0.10)
CFLAGS=-O0 -g $(GST_FLAGS)

$(TARGET) : $(OBJS)
	gcc -o $@ $(OBJS) $(shell pkg-config --libs gstreamer-0.10)

%.o: %.c
	gcc $(CFLAGS)\
	$(INCS:%=-I%) $(SYSINCS:%=-isystem%) -c \
	-MMD -MP -MF"$(@:$.o=$.d)" -MT"$@" \
	-o $@ $<

clean :
	rm -rf *.o *.d *~ core $(TARGET) *.html

.PHONY: all clean
.SUFFIXES: