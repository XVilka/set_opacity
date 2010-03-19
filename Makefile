PACKAGES = xcomposite xfixes xdamage xrender
LIBS=`pkg-config --libs ${PACKAGES}` -lm
INCS=`pkg-config --cflags ${PACKAGES}`

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $*.c

OBJS=set_opacity.o interface.o stack.o

set_opacity: $(OBJS)
	$(CC) $(CFLAGS) -g -o $@ $(OBJS) $(LIBS)

$(OBJS): interface.h stack.h

clean:
	rm -f $(OBJS) set_opacity
