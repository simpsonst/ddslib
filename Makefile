all::

CD=cd
CAT=cat
AR=ar
RANLIB=ranlib
INSTALL=install
MKDIR=mkdir
FIND=find
CP=cp
RISCOS_ZIP=zip
ARCHIVE_DIR=$(HOME)
ENABLE_CXX=yes

# INSTALL_PATH is deprecated
INSTALL_PATH=/usr/local
PREFIX=$(INSTALL_PATH)

-include ddslib-env.mk

HEADERS=dllist.h btree.h bheap.h internal.h
COMPAT_HEADERS=dllist.h btree.h bheap.h
SOURCES=bheap.c
LIBRARIES=ddslib

ifeq ($(ENABLE_CXX),yes)
HEADERS += dllist.hh
endif

ddslib_mod=bheap.o

testheap_obj=testheap.o libddslib.a

all:: $(LIBRARIES:%=lib%.a)

install:: install-headers install-libraries

$(PREFIX)/include:
	$(INSTALL) -d $(PREFIX)/include

$(PREFIX)/include/ddslib:
	$(INSTALL) -d $(PREFIX)/include/ddslib

$(PREFIX)/lib:
	$(INSTALL) -d $(PREFIX)/lib

install-headers: $(PREFIX)/include $(PREFIX)/include/ddslib $(HEADERS)
	$(INSTALL) -m 0644 $(HEADERS) $(PREFIX)/include/ddslib
	$(INSTALL) -m 0644 $(COMPAT_HEADERS:%=compat/%) $(PREFIX)/include

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
	-$(RM) testree testheap
	-$(RM) $(LIBRARIES:%=lib%.a)

testheap: $(testheap_obj)
	$(LINK.c) -o $@ $(testheap_obj) $(testheap_lib)

testheap.o: bheap.h
testree.o: btree.h
bheap.o: bheap.h

install-apps:: \
	$(PREFIX)/apps/!DDSLib/!Boot,feb \
	$(PREFIX)/apps/!DDSLib/README,faf \
	$(PREFIX)/apps/!DDSLib/COPYING,fff \
	$(PREFIX)/apps/!DDSLib/VERSION,fff \
	$(PREFIX)/apps/!DDSLib/HISTORY,fff \
	$(COMPAT_HEADERS:%.h=$(PREFIX)/apps/!DDSLib/Library/h/%,fff) \
	$(HEADERS:%.h=$(PREFIX)/apps/!DDSLib/Library/ddslib/h/%,fff) \
	$(HEADERS:%.hh=$(PREFIX)/apps/!DDSLib/Library/ddslib/hh/%,fff) \
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
