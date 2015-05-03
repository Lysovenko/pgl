#makefil
GTKLIBS = `pkg-config gtk+-2.0 gthread-2.0 --libs`
GTKFLAGS = `pkg-config gtk+-2.0 --cflags`
QINC = -I$(HOME)/include
DEBUG = -ggdb -Wall
# $(LIBS) $(CFLAGS)
OBJS = gdk_plotpr.o primitives.o ps_plotpr.o svg_plotpr.o area_plotgr.o
all: libpgl.so test
libpgl.so: $(OBJS)
	gcc --shared $(GTKLIBS) $(OBJS) -o libpgl.so $(DEBUG)
%.o : %.c
	gcc -c $< -fPIC $(DEBUG)
gdk_plotpr.o:gdk_plotpr.c
	gcc -c gdk_plotpr.c  $(GTKFLAGS) -fPIC $(DEBUG)

test: test.c
	gcc -o test  test.c $(QINC) `echo $$LD_LIBRARY_PATH | sed -e 's/^/-L/;s/:/ -L/g'` -lpgl $(GTKLIBS) $(GTKFLAGS) $(DEBUG)
#.PHONY: test testel gtk sbs show
