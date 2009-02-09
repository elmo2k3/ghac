CFLAGS=`pkg-config --cflags libglade-2.0` -Wall -fPIC -g -D_DEBUG
LDFLAGS=`pkg-config --libs libglade-2.0` -export-dynamic -lhac -lmysqlclient

ghac: ghac.o graph_view.o libhagraph.o data.o config.o

clean:
	rm *.o ghac
