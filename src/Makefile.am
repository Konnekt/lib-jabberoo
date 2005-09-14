INCLUDES = -I$(top_srcdir)/libjudo/src/expat \
	   -I$(top_srcdir)/libjudo/src \
           -I$(top_srcdir)/include $(SIGC_CFLAGS)

jabberoo_sources = jutil.cc \
	jabberoo-packet.cc \
    jabberoo-disco.cpp \
    jabberoo-filestream.cc \
	jabberoo-session.cc \
	jabberoo-message.cc \
	jabberoo-presence.cc \
	jabberoo-presencedb.cc \
	jabberoo-roster.cc \
	jabberoo-JID.cc \
	jabberoox-filter.cc \
	jabberoox-agent.cc \
    jabberoox-vcard.cc \
    jabberoo-XCP.cc \
	sha.c

lib_LTLIBRARIES = libjabberoo.la
libjabberoo_la_SOURCES = $(jabberoo_sources)
libjabberoo_la_LIBADD = ../libjudo/src/libjudo.la
libjabberoo_la_LIBS = $(SIGC_LIBS)
