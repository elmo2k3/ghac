CFLAGS=`pkg-config --cflags libglade-2.0` -Wall
LDFLAGS=`pkg-config --libs libglade-2.0` -export-dynamic

main: main.o

clean:
	rm *.o main
