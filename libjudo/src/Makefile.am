
noinst_LTLIBRARIES = libjudo.la

libjudo_la_SOURCES = judo.hpp \
                     judo.cpp \
                     Element.cpp \
                     ElementStream.cpp \
                     XPath.cpp \
                     XPath.h \
                     XPathFunctions.h \
                     XPathOps.h

libjudo_la_LIBADD = ./expat/libexpat.la

libjudodir = $(includedir)/jabberoo
libjudo_HEADERS = judo.hpp XPath.h XPathFunctions.h XPathOps.h

INCLUDES = -I$(srcdir)/expat \
           -I.

if TEST
SUBDIRS = expat . test
else
SUBDIRS = expat . 
endif
