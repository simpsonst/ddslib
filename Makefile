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

# INSTALL_PATH is deprecated
INSTALL_PATH=/usr/local
PREFIX=$(INSTALL_PATH)

-include ddslib-env.mk

HEADERS=alarm.h alarmsrc.h dllist.h btree.h bheap.h
SOURCES=bheap.c
LIBRARIES=ddslib

ddslib_mod=bheap.o

testheap_obj=testheap.o libddslib.a

all:: $(LIBRARIES:%=lib%.a)

install:: install-headers install-libraries

$(PREFIX)/include:
	$(INSTALL) -d $(PREFIX)/include

$(PREFIX)/lib:
	$(INSTALL) -d $(PREFIX)/lib

install-headers: $(PREFIX)/include $(HEADERS)
	$(INSTALL) -m 0644 $(HEADERS) $(PREFIX)/include

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
	-$(RM) testree testalarm testheap
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
	$(HEADERS:%.h=$(PREFIX)/apps/!DDSLib/h/%,fff) \
	$(SOURCES:%.c=$(PREFIX)/apps/!DDSLib/c/%,fff) \
	$(LIBRARIES:%=$(PREFIX)/apps/!DDSLib/o/%,ffd)

$(PREFIX)/apps/!DDSLib/%,faf: %.html
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/%,fff: %
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/%: package/%
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/h/%,fff: %.h
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/c/%,fff: %.c
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

$(PREFIX)/apps/!DDSLib/o/%,ffd: lib%.a
	$(MKDIR) -p "$(@D)"
	$(CP) "$<" "$@"

riscos-archive: install-apps
	FILENAME="$(ARCHIVE_DIR)/ddslib-riscos-`$(CAT) VERSION`.zip" ; \
	$(CD) $(PREFIX)/apps ; \
	$(RM) "$$FILENAME" ; \
	$(RISCOS_ZIP) -, -r "$$FILENAME" *
