CFLAGS=`pkg-config --cflags libglade-2.0` -Wall $(INCDIRS)
LDFLAGS=`pkg-config --libs libglade-2.0` -export-dynamic -lhac -lhagraph

ghac: ghac.o graph_view.o

clean:
	rm *.o ghac
