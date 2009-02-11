CFLAGS=`pkg-config --cflags libglade-2.0` `pkg-config --cflags libhac` -Wall -fPIC -g -D_DEBUG
LDFLAGS=`pkg-config --libs libglade-2.0` `pkg-config --libs libhac`  -export-dynamic -lhac -lmysqlclient

ghac: ghac.o graph_view.o libhagraph.o data.o config.o

clean:
	rm *.o ghac
