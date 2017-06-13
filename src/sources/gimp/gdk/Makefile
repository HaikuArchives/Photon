# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.3 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DISTDIR =

pkgdatadir = $(datadir)/gtk+
pkglibdir = $(libdir)/gtk+
pkgincludedir = $(includedir)/gtk+

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = i586-pc-linux-gnu
host_triplet = i586-pc-linux-gnu
CC = gcc
GTK_MAJOR_VERSION = 1
GTK_MICRO_VERSION = 6
GTK_MINOR_VERSION = 0
GTK_VERSION = 1.0.6
LD = /usr/bin/ld
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAINT = #M#
MAKEINFO = makeinfo
NM = /usr/bin/nm -B
PACKAGE = gtk+
RANLIB = ranlib
VERSION = 1.0.6
x_cflags =  -I/usr/X11R6/include
x_includes = /usr/X11R6/include
x_ldflags =   -L/usr/X11R6/lib
x_libs = -lXext -lX11 
xinput_progs = 

gdkincludedir = $(includedir)/gdk

lib_LTLIBRARIES = libgdk.la

libgdk_la_SOURCES = \
	gdk.c		\
	gdkcc.c		\
	gdkcolor.c	\
	gdkcursor.c	\
	gdkdnd.c	\
	gdkdraw.c	\
	gdkfont.c	\
	gdkgc.c		\
	gdkglobals.c	\
	gdkimage.c	\
	gdkinput.c	\
	gdkinput.h	\
	gdkinputnone.h  \
	gdkinputcommon.h\
	gdkinputgxi.h	\
	gdkinputxfree.h	\
	gdkpixmap.c	\
	gdkproperty.c	\
	gdkrectangle.c	\
	gdkregion.c	\
	gdkselection.c	\
	gdkvisual.c	\
	gdkwindow.c	\
	gdkxid.c 	\
	MwmUtil.h       \
	gxid_lib.h	\
	gxid_proto.h	\
        gxid_lib.c

gdkinclude_HEADERS = \
	gdk.h		\
	gdkcursors.h	\
 	gdki18n.h	\
	gdkkeysyms.h	\
	gdkprivate.h	\
	gdktypes.h	\
	gdkx.h

libgdk_la_LDFLAGS = -version-info $(GTK_MAJOR_VERSION):$(GTK_MICRO_VERSION):0 \
	  -L/usr/X11R6/lib -lXext -lX11 

INCLUDES = -I$(top_srcdir) -I../glib -I$(top_srcdir)/glib  -I/usr/X11R6/include 

EXTRA_PROGRAMS = gxid

bin_PROGRAMS = 

gxid_SOURCES = gxid.c

gxid_LDADD = \
	  -L/usr/X11R6/lib	\
	-lXext -lX11 	\
	-lm

BUILT_SOURCES = gdkcursors.h gdkkeysyms.h

EXTRA_DIST = makecursors.awk makekeysyms.awk
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(lib_LTLIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I..
CPPFLAGS = 
LDFLAGS = 
LIBS = 
X_CFLAGS =  -I/usr/X11R6/include
X_LIBS =  -L/usr/X11R6/lib
X_EXTRA_LIBS = 
X_PRE_LIBS =  -lSM -lICE
libgdk_la_LIBADD = 
libgdk_la_OBJECTS =  gdk.lo gdkcc.lo gdkcolor.lo gdkcursor.lo gdkdnd.lo \
gdkdraw.lo gdkfont.lo gdkgc.lo gdkglobals.lo gdkimage.lo gdkinput.lo \
gdkpixmap.lo gdkproperty.lo gdkrectangle.lo gdkregion.lo \
gdkselection.lo gdkvisual.lo gdkwindow.lo gdkxid.lo gxid_lib.lo
PROGRAMS =  $(bin_PROGRAMS)

gxid_OBJECTS =  gxid.o
gxid_DEPENDENCIES = 
gxid_LDFLAGS = 
CFLAGS = -g -O2 -DGTK_NO_CHECK_CASTS -Wall -DUSE_XIM
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LINK = $(LIBTOOL) --mode=link $(CC) $(CFLAGS) $(LDFLAGS) -o $@
HEADERS =  $(gdkinclude_HEADERS)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
SOURCES = $(libgdk_la_SOURCES) $(gxid_SOURCES)
OBJECTS = $(libgdk_la_OBJECTS) $(gxid_OBJECTS)

all: Makefile $(LTLIBRARIES) $(PROGRAMS) $(HEADERS)

.SUFFIXES:
.SUFFIXES: .S .c .lo .o .s
$(srcdir)/Makefile.in: #M# Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps gdk/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libLTLIBRARIES:

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)

distclean-libLTLIBRARIES:

maintainer-clean-libLTLIBRARIES:

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "$(LIBTOOL)  --mode=install $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

.c.o:
	$(COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:

libgdk.la: $(libgdk_la_OBJECTS) $(libgdk_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libgdk_la_LDFLAGS) $(libgdk_la_OBJECTS) $(libgdk_la_LIBADD) $(LIBS)

mostlyclean-binPROGRAMS:

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

distclean-binPROGRAMS:

maintainer-clean-binPROGRAMS:

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	list='$(bin_PROGRAMS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`; \
	done

gxid: $(gxid_OBJECTS) $(gxid_DEPENDENCIES)
	@rm -f gxid
	$(LINK) $(gxid_LDFLAGS) $(gxid_OBJECTS) $(gxid_LDADD) $(LIBS)

install-gdkincludeHEADERS: $(gdkinclude_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(gdkincludedir)
	@list='$(gdkinclude_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(gdkincludedir)/$$p"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(gdkincludedir)/$$p; \
	done

uninstall-gdkincludeHEADERS:
	@$(NORMAL_UNINSTALL)
	list='$(gdkinclude_HEADERS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(gdkincludedir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $(SOURCES) $(HEADERS) $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = gdk

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
gdk.lo gdk.o: gdk.c ../config.h gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h gdkinput.h \
	gdkx.h gdkprivate.h gdkkeysyms.h gdki18n.h
gdkcc.lo gdkcc.o: gdkcc.c gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h gdkx.h \
	gdkprivate.h
gdkcolor.lo gdkcolor.o: gdkcolor.c gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkcursor.lo gdkcursor.o: gdkcursor.c gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkdnd.lo gdkdnd.o: gdkdnd.c gdkx.h gdkprivate.h gdktypes.h \
	../glib/glib.h ../glib/glibconfig.h gdkcursors.h gdk.h
gdkdraw.lo gdkdraw.o: gdkdraw.c gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkfont.lo gdkfont.o: gdkfont.c gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkgc.lo gdkgc.o: gdkgc.c gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkglobals.lo gdkglobals.o: gdkglobals.c gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h gdktypes.h
gdkimage.lo gdkimage.o: gdkimage.c ../config.h gdk.h gdktypes.h \
	../glib/glib.h ../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkinput.lo gdkinput.o: gdkinput.c ../config.h gdk.h gdktypes.h \
	../glib/glib.h ../glib/glibconfig.h gdkcursors.h gdkx.h \
	gdkprivate.h gdkprivate.h gdkinput.h gdkinputnone.h \
	gdkinputcommon.h gdkinputxfree.h gdkinputgxi.h
gdkpixmap.lo gdkpixmap.o: gdkpixmap.c ../config.h gdk.h gdktypes.h \
	../glib/glib.h ../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkproperty.lo gdkproperty.o: gdkproperty.c gdk.h gdktypes.h \
	../glib/glib.h ../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkrectangle.lo gdkrectangle.o: gdkrectangle.c gdk.h gdktypes.h \
	../glib/glib.h ../glib/glibconfig.h gdkcursors.h
gdkregion.lo gdkregion.o: gdkregion.c gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkselection.lo gdkselection.o: gdkselection.c gdk.h gdktypes.h \
	../glib/glib.h ../glib/glibconfig.h gdkcursors.h gdkprivate.h \
	gdkx.h gdkprivate.h
gdkvisual.lo gdkvisual.o: gdkvisual.c gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h gdkprivate.h
gdkwindow.lo gdkwindow.o: gdkwindow.c gdk.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h ../config.h gdkinput.h \
	gdkprivate.h MwmUtil.h
gdkxid.lo gdkxid.o: gdkxid.c gdkprivate.h gdktypes.h ../glib/glib.h \
	../glib/glibconfig.h gdkcursors.h
gxid_lib.lo gxid_lib.o: gxid_lib.c ../config.h gxid_lib.h gxid_proto.h

info:
dvi:
check: all
	$(MAKE)
installcheck:
install-exec: install-libLTLIBRARIES install-binPROGRAMS
	@$(NORMAL_INSTALL)

install-data: install-gdkincludeHEADERS
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-libLTLIBRARIES uninstall-binPROGRAMS uninstall-gdkincludeHEADERS

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(DATADIR)$(libdir) $(DATADIR)$(bindir) \
		$(DATADIR)$(gdkincludedir)


mostlyclean-generic:
	-test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(DISTCLEANFILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
	-test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
mostlyclean:  mostlyclean-libLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-binPROGRAMS \
		mostlyclean-tags mostlyclean-generic

clean:  clean-libLTLIBRARIES clean-compile clean-libtool \
		clean-binPROGRAMS clean-tags clean-generic mostlyclean

distclean:  distclean-libLTLIBRARIES distclean-compile distclean-libtool \
		distclean-binPROGRAMS distclean-tags distclean-generic \
		clean
	-rm -f config.status
	-rm -f libtool

maintainer-clean:  maintainer-clean-libLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-binPROGRAMS maintainer-clean-tags \
		maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: mostlyclean-libLTLIBRARIES distclean-libLTLIBRARIES \
clean-libLTLIBRARIES maintainer-clean-libLTLIBRARIES \
uninstall-libLTLIBRARIES install-libLTLIBRARIES mostlyclean-compile \
distclean-compile clean-compile maintainer-clean-compile \
mostlyclean-libtool distclean-libtool clean-libtool \
maintainer-clean-libtool mostlyclean-binPROGRAMS distclean-binPROGRAMS \
clean-binPROGRAMS maintainer-clean-binPROGRAMS uninstall-binPROGRAMS \
install-binPROGRAMS uninstall-gdkincludeHEADERS \
install-gdkincludeHEADERS tags mostlyclean-tags distclean-tags \
clean-tags maintainer-clean-tags distdir info dvi installcheck \
install-exec install-data install uninstall all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


gdkcursors.h:
	awk -f $(srcdir)/makecursors.awk /usr/X11R6/include/X11/cursorfont.h > $@

gdkkeysyms.h:
	awk -f $(srcdir)/makekeysyms.awk /usr/X11R6/include/X11/keysymdef.h > $@

.PHONY: files

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
