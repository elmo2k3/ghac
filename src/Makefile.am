bin_PROGRAMS = ghac

ghac_SOURCES = configfile.c\
	ghac.c\
	graph_view.c

if WIN32
ghac_LDADD = icon.o
endif

LIBS = @gtk_LIBS@ -export-dynamic @EXTRA_LDFLAGS@
AM_CFLAGS = @CFLAGS@ @gtk_CFLAGS@ @EXTRA_CFLAGS@

if ENABLE_LIBHAGRAPH
LIBS+=@libhagraph_LIBS@
AM_CFLAGS+=@libhagraph_CFLAGS@
if WIN32
LIBS+= -lmysql
else
LIBS+= -lmysqlclient
endif
endif

if ENABLE_LIBHAC
LIBS+=@libhac_LIBS@
AM_CFLAGS+=@libhac_CFLAGS@
endif

if WIN32
LIBS += -lws2_32 -lpthreadGC2 -lssl32 -leay32
else
LIBS+= @libssl_LIBS@
AM_CFLAGS+= @libssl_CFLAGS@
endif

