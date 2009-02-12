bin_PROGRAMS = ghac

ghac_SOURCES = config.c\
	ghac.c\
	graph_view.c

LIBS = @libglade_LIBS@ @libhagraph_LIBS@ @libhac_LIBS@

if WIN32
LIBS += -lmysql -lws2_32 -lpthreadGC2
else
LIBS += -lmysqlclient
endif

AM_CFLAGS = @CFLAGS@ @libglade_CFLAGS@ @libhagraph_CFLAGS@ @libhac_CFLAGS@
