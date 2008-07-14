INCDIRS = -I../libhac/
CFLAGS=`pkg-config --cflags libglade-2.0` -Wall $(INCDIRS)
LDFLAGS=`pkg-config --libs libglade-2.0` -export-dynamic -lhac

ghac: ghac.o

clean:
	rm *.o ghac
