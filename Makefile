all: testprint libuniqueptr.so

GLIBFLAGS=`pkg-config --cflags --libs glib-2.0`
CFLAGS=-Wall -O2

libuniqueptr.so: printf_unique_ptr.c
	gcc -fPIC -rdynamic -shared -o libuniqueptr.so printf_unique_ptr.c $(CFLAGS)  $(GLIBFLAGS)

testprint: testprint.c
	gcc -o testprint testprint.c  $(CFLAGS) $(GLIBFLAGS)

check: testprint libuniqueptr.so
	./testprint
	LD_PRELOAD=./libuniqueptr.so ./testprint

clean:
	rm -f testprint libuniqueptr.so
