bin_PROGRAMS = ghac

ghac_SOURCES = config.c\
	ghac.c\
	graph_view.c

LIBS = @libglade_LIBS@ @libhagraph_LIBS@ @libhac_LIBS@ -lmysqlclient
AM_CFLAGS = @CFLAGS@ @libglade_CFLAGS@ @libhagraph_CFLAGS@ @libhac_CFLAGS@
