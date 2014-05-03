# Hey emacs, this is a -*- makefile -*-
#
# This Makefile controls which record libraries are built.  The standard 
# ScalaTrace library does inter- and intra-node compression.  Uncomment 
# other build diectories below to enable variants.
#

BUILDDIRS=

#
# Standard ScalaTrace with inter- and intra-node compression.
#
BUILDDIRS += libsrc/glob

#
# ScalaTrace library with per-node compressed traces, but
# no inter-node compression.
#
BUILDDIRS += libsrc/node

#
# ScalaTrace with no compression. Outputs per-node, uncompressed traces.
#
BUILDDIRS += libsrc/dump

#
# Variables and targets below should not need modification.
#
WRAPPERDIR = wrapper-engine
SUBDIRS = ${WRAPPERDIR} ${BUILDDIRS}
TESTS   = 

include ./config/Makefile.config

all:
	(for i in $(SUBDIRS); do (cd $$i; $(MAKE) $@) || break; done)

dump:
	(cd libsrc/dump; $(MAKE)) || break

node:
	(cd libsrc/node; $(MAKE)) || break

glob:
	(cd libsrc/glob; $(MAKE)) || break

test:
	(for i in $(TESTS); do (cd $$i; $(MAKE) ) || break; done)

clean:
	$(RM) *~ a.out
	rm -fr ./lib/libumpire.a
	(for i in $(BUILDDIRS); do (cd $$i; $(MAKE) $@); done)
	(cd wrapper-engine && make clean)
	(rm -f `find . -name "*~"`)

realclean:
	$(RM) *~ a.out
	rm -fr ./lib/libumpire.a
	(for i in $(SUBDIRS) $(TESTS); do (cd $$i; $(MAKE) clean); done)

