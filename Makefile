all::

PRINTF=printf
FIND=find
SED=sed
XARGS=xargs

ENABLE_CXX=yes
ENABLE_C99=yes

## INSTALL_PATH is deprecated.
INSTALL_PATH=/usr/local
PREFIX=$(INSTALL_PATH)

-include ddslib-env.mk

lc=$(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

test_binaries.c += testvstr
test_binaries.c += testhash
test_binaries.c += testheap
test_binaries.c += testree

libraries += ddslib

DDSLIB_HEADERS += dllist.h
DDSLIB_HEADERS += btree.h
DDSLIB_HEADERS += bheap.h
DDSLIB_HEADERS += internal.h

COMPAT_HEADERS += dllist.h
COMPAT_HEADERS += btree.h
COMPAT_HEADERS += bheap.h

ddslib_mod += bheap

ifneq ($(filter true t y yes on 1,$(call lc,$(ENABLE_C99))),)
DDSLIB_HEADERS += vstr.h
DDSLIB_HEADERS += vwcs.h
DDSLIB_HEADERS += htab.h

ddslib_mod += htab
ddslib_mod += vstr
ddslib_mod += vwcs
endif

ifneq ($(filter true t y yes on 1,$(call lc,$(ENABLE_CXX))),)
DDSLIB_HEADERS += dllist.hh
endif

headers += $(DDSLIB_HEADERS:%=ddslib/%)
headers += $(COMPAT_HEADERS)

testheap_obj += testheap
testheap_obj += bheap

testhash_obj += testhash
testhash_obj += htab

testvstr_obj += testvstr
testvstr_obj += vstr
testvstr_obj += vwcs

riscos_apps += ddslib
ddslib_appname=!DDSLib

c=,

SOURCES=$(filter-out $(headers),$(patsubst src/obj/%,%,$(wildcard src/obj/*.c src/obj/*.h src/obj/ddslib/*.h)))

ddslib_app += !Boot,feb
ddslib_app += README,faf
ddslib_app += COPYING,fff
ddslib_app += VERSION,fff
ddslib_app += HISTORY,fff
ddslib_app += $(COMPAT_HEADERS:%.h=Library/h/%,fff)
ddslib_app += $(patsubst %.h,Library/ddslib/h/%$cfff,$(filter %.h,$(DDSLIB_HEADERS)))
ddslib_app += $(patsubst %.hh,Library/ddslib/hh/%$cfff,$(filter %.hh,$(DDSLIB_HEADERS)))
ddslib_app += $(patsubst %.c,Source/c/%$cfff,$(filter %.c,$(SOURCES)))
ddslib_app += $(patsubst %.h,Source/h/%$cfff,$(filter %.h,$(SOURCES)))
ddslib_app += $(libraries:%=Library/o/%,ffd)

include binodeps.mk

all:: $(libraries:%=out/lib%.a)

install:: install-headers install-libraries

ifneq ($(filter true t y yes on 1,$(call lc,$(ENABLE_RISCOS))),)
install:: install-riscos
all:: out/ddslib-riscos.zip
endif


# Set this to the comma-separated list of years that should appear in
# the licence.  Do not use characters other than [0-9,] - no spaces.
YEARS=2002-3,2005-6,2012,2016

update-licence:
	$(FIND) . -name ".svn" -prune -or -type f -print0 | $(XARGS) -0 \
	$(SED) -i 's/Copyright (C) [0-9,-]\+  Steven Simpson/Copyright (C) $(YEARS)  Steven Simpson/g'


tidy::
	@$(FIND) . -name "*~" -delete
