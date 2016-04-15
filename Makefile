all::

CD=cd
CAT=cat
AR=ar
RANLIB=ranlib
INSTALL=install
MKDIR=mkdir
FIND=find
SED=sed
XARGS=xargs
CP=cp
RISCOS_ZIP=zip
ARCHIVE_DIR=$(HOME)
ENABLE_CXX=yes
ENABLE_C99=yes

# INSTALL_PATH is deprecated
INSTALL_PATH=/usr/local
PREFIX=$(INSTALL_PATH)

-include ddslib-env.mk

HEADERS=dllist.h btree.h bheap.h internal.h
COMPAT_HEADERS=dllist.h btree.h bheap.h
SOURCES=bheap.c
LIBRARIES=ddslib

ifeq ($(ENABLE_C99),yes)
HEADERS += htab.h vstr.h vwcs.h
endif

ifeq ($(ENABLE_CXX),yes)
HEADERS += dllist.hh
endif

ddslib_mod=bheap.o

ifeq ($(ENABLE_C99),yes)
ddslib_mod += htab.o vstr.o vwcs.o
endif

testheap_obj=testheap.o bheap.o
testhash_obj=testhash.o htab.o
testvstr_obj=testvstr.o vstr.o vwcs.o

all:: $(LIBRARIES:%=lib%.a)

install:: install-headers install-libraries

$(PREFIX)/include:
	$(INSTALL) -d $(PREFIX)/include

$(PREFIX)/include/ddslib:
	$(INSTALL) -d $(PREFIX)/include/ddslib

$(PREFIX)/lib:
	$(INSTALL) -d $(PREFIX)/lib

install-headers: $(PREFIX)/include $(PREFIX)/include/ddslib \
		$(HEADERS:%=ddslib/%)
	$(INSTALL) -m 0644 $(HEADERS:%=ddslib/%) $(PREFIX)/include/ddslib
	$(INSTALL) -m 0644 $(COMPAT_HEADERS) $(PREFIX)/include

install-libraries: $(PREFIX)/lib $(LIBRARIES:%=lib%.a)
	$(INSTALL) -m 0644 $(LIBRARIES:%=lib%.a) $(PREFIX)/lib

libddslib.a: $(ddslib_mod)
	$(AR) r $@ $(ddslib_mod)
	$(RANLIB) $@

### Housekeeping

tidy:
	-$(FIND) . -name "*~" -exec $(RM) {} \;
	-$(RM) core

clean: tidy
	-$(RM) *.o

blank: clean
	-$(RM) testree testheap testhash
	-$(RM) $(LIBRARIES:%=lib%.a)

testheap: $(testheap_obj)
	$(LINK.c) -o $@ $(testheap_obj) $(testheap_lib)

testhash: $(testhash_obj)
	$(LINK.c) -o $@ $(testhash_obj) $(testhash_lib)

testvstr: $(testvstr_obj)
	$(LINK.c) -o $@ $(testvstr_obj) $(testvstr_lib)

bheap.o: ddslib/bheap.h
htab.o: ddslib/htab.h
testhash.o: ddslib/htab.h
testheap.o: ddslib/bheap.h
testree.o: ddslib/btree.h
testvstr.o vstr.o: ddslib/vstr.h
testvstr.o vwcs.o vwcsx.o: ddslib/vwcs.h

vwcs.o vstr.o: vimpl.h

HEADERS_C=$(filter %.h,$(HEADERS))
HEADERS_CXX=$(filter %.hh,$(HEADERS))

install-apps:: \
	$(PREFIX)/apps/!DDSLib/!Boot,feb \
	$(PREFIX)/apps/!DDSLib/README,faf \
	$(PREFIX)/apps/!DDSLib/COPYING,fff \
	$(PREFIX)/apps/!DDSLib/VERSION,fff \
	$(PREFIX)/apps/!DDSLib/HISTORY,fff \
	$(COMPAT_HEADERS:%.h=$(PREFIX)/apps/!DDSLib/Library/h/%,fff) \
	$(HEADERS_C:%.h=$(PREFIX)/apps/!DDSLib/Library/ddslib/h/%,fff) \
	$(HEADERS_CXX:%.hh=$(PREFIX)/apps/!DDSLib/Library/ddslib/hh/%,fff) \
	$(SOURCES:%.c=$(PREFIX)/apps/!DDSLib/Source/c/%,fff) \
	$(LIBRARIES:%=$(PREFIX)/apps/!DDSLib/Library/o/%,ffd)

$(PREFIX)/apps/!DDSLib/%,faf: %.html
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/%,fff: %
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/%: package/%
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/Library/h/%,fff: %.h
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/Library/ddslib/h/%,fff: ddslib/%.h
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/Library/ddslib/hh/%,fff: ddslib/%.hh
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/Source/c/%,fff: %.c
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/Library/o/%,ffd: lib%.a
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

riscos-archive: install-apps
	FILENAME="$(ARCHIVE_DIR)/ddslib-riscos-`$(CAT) VERSION`.zip" ; \
	$(CD) $(PREFIX)/apps ; \
	$(RM) "$$FILENAME" ; \
	$(RISCOS_ZIP) -, -r "$$FILENAME" !DDSLib


# Set this to the comma-separated list of years that should appear in
# the licence.  Do not use characters other than [0-9,] - no spaces.
YEARS=2002-3,2005-6,2012

update-licence:
	$(FIND) . -name ".svn" -prune -or -type f -print0 | $(XARGS) -0 \
	$(SED) -i 's/Copyright (C) [0-9,-]\+  Steven Simpson/Copyright (C) $(YEARS)  Steven Simpson/g'
