 ## Makefile.am -- Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libmtapi.la

MRAPI_CFLAGS = -I$(top_srcdir)/mrapi/include -I$(top_srcdir)/mrapi/src/mrapi_impl/sysvr4 
MCAPI_CFLAGS = -I$(top_srcdir)/mcapi/include -I$(top_srcdir)/mcapi/src/mcapi_trans/mcapi_trans_sm  
MTAPI_CFLAGS = -fno-strict-aliasing

INCLUDES = -I$(top_srcdir)/mtapi/include -I$(top_srcdir) $(MRAPI_CFLAGS) $(MCAPI_CFLAGS) $(MTAPI_CFLAGS) 

include_HEADERS = $(top_srcdir)/mca.h $(top_srcdir)/mtapi/include/mtapi.h $(top_srcdir)/mca_config.h $(top_srcdir)/mtapi/include/mtapi_runtime.h $(top_srcdir)/mtapi/include/queue.h $(top_srcdir)/mtapi/include/linklist.h  $(top_srcdir)/mtapi/include/mutex.h

libmtapi_la_SOURCES = mtapi.c mutex.c mtapi_runtime.c queue.c linklist.c 

install_libs = libmtapi

#SYSVR4 = $(shell pwd)
