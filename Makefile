all::

TAR=tar
CD=cd
CAT=cat
AR=ar
RANLIB=ranlib
INSTALL=install
MKDIR=mkdir -p
FIND=find
SED=sed
XARGS=xargs
CP=cp
ZIP=zip
PRINTF=printf
CMP=cmp -s
CPCMP=$(CMP) '$(1)' '$(2)' || ( $(CP) '$(1)' '$(2)' && $(PRINTF) '[Changed]: %s\n' '$(3)' )

DEPARGS.c=-MMD -MT '$(1)' -MF '$(2)'
DEPARGS.cc=-MMD -MT '$(1)' -MF '$(2)'

ENABLE_CXX=yes
ENABLE_C99=yes

## INSTALL_PATH is deprecated.
INSTALL_PATH=/usr/local
PREFIX=$(INSTALL_PATH)

-include ddslib-env.mk

lc=$(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

CBINARIES += testvstr
CBINARIES += testhash
CBINARIES += testheap
CBINARIES += testree

LIBRARIES += ddslib

HEADERS += dllist.h
HEADERS += btree.h
HEADERS += bheap.h
HEADERS += internal.h

COMPAT_HEADERS += dllist.h
COMPAT_HEADERS += btree.h
COMPAT_HEADERS += bheap.h

ddslib_mod += bheap

ifneq ($(filter true t y yes on 1,$(call lc,$(ENABLE_C99))),)
HEADERS += vstr.h
HEADERS += vwcs.h
HEADERS += htab.h

ddslib_mod += htab
ddslib_mod += vstr
ddslib_mod += vwcs
endif

ifneq ($(filter true t y yes on 1,$(call lc,$(ENABLE_CXX))),)
HEADERS += dllist.hh
endif

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

SOURCES=$(filter-out $(HEADERS:%=ddslib/%) $(COMPAT_HEADERS),$(patsubst src/obj/%,%,$(wildcard src/obj/*.c src/obj/*.h src/obj/ddslib/*.h)))

ddslib_app += !Boot,feb
ddslib_app += README,faf
ddslib_app += COPYING,fff
ddslib_app += VERSION,fff
ddslib_app += HISTORY,fff
ddslib_app += $(COMPAT_HEADERS:%.h=Library/h/%,fff)
ddslib_app += $(patsubst %.h,Library/ddslib/h/%$cfff,$(filter %.h,$(HEADERS)))
ddslib_app += $(patsubst %.hh,Library/ddslib/hh/%$cfff,$(filter %.hh,$(HEADERS)))
ddslib_app += $(patsubst %.c,Source/c/%$cfff,$(filter %.c,$(SOURCES)))
ddslib_app += $(patsubst %.h,Source/h/%$cfff,$(filter %.h,$(SOURCES)))
ddslib_app += $(LIBRARIES:%=Library/o/%,ffd)


all:: $(LIBRARIES:%=out/lib%.a)

install:: install-headers install-libraries

ifneq ($(filter true t y yes on 1,$(call lc,$(ENABLE_RISCOS))),)
install:: install-riscos
all:: out/ddslib-riscos.zip
endif

install-headers:
	@$(PRINTF) 'Installing headers in %s:\n' '$(PREFIX)/include'
	@$(PRINTF) '\t<%s>\n' $(HEADERS:%=ddslib/%) $(COMPAT_HEADERS)
	@$(INSTALL) -d $(PREFIX)/include/ddslib
	@$(INSTALL) -m 0644 $(HEADERS:%=src/obj/ddslib/%) \
	  $(PREFIX)/include/ddslib
	@$(INSTALL) -m 0644 $(COMPAT_HEADERS:%=src/obj/%) \
	  $(PREFIX)/include

install-libraries:
	@$(PRINTF) 'Installing libraries in %s:\n' '$(PREFIX)/lib'
	@$(PRINTF) '\t%s\n' $(LIBRARIES)
	@$(INSTALL) -d $(PREFIX)/lib
	@$(INSTALL) -m 0644 $(LIBRARIES:%=out/lib%.a) $(PREFIX)/lib

install-riscos::
	@$(PRINTF) 'Installing RISC OS apps in %s:\n' '$(PREFIX)/apps'
	@$(PRINTF) '\t%s\t(%s)\n' $(foreach app,$(riscos_apps),'$(app)' '$($(app)_appname)')
	@$(INSTALL) -d $(PREFIX)/apps
	@$(TAR) cf - -C out/riscos \
	  $(foreach app,$(riscos_apps),$($(app)_app:%=$($(app)_appname)/%)) | \
	  $(TAR) xf - -C $(PREFIX)/apps


## Generic rules

SOURCE_DIRS:=$(shell $(FIND) src/obj -type d -printf '%P')

BINARIES += $(CBINARIES)
BINARIES += $(CXXBINARIES)

ALL_OBJECTS=$(sort $(foreach prog,$(BINARIES),$($(prog)_obj)) $(foreach lib,$(LIBRARIES),$($(lib)_mod)))



define PROGOBJLIST_RULES
tmp/progobjlisted.$(1):
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) > 'tmp/progobjlist.$(1)-tmp' '%s\n' $$(sort $$($(1)_obj))

tmp/progobjlist.$(1): | tmp/progobjlisted.$(1)

endef

.IDLE: $(BINARIES:%=tmp/progobjlisted.%)
.IDLE: $(BINARIES:%=tmp/progobjlist.%)

tmp/progobjlist.%: tmp/progobjlisted.%
	@$(call CPCMP,$@-tmp,$@,$* object list)

$(foreach prog,$(BINARIES),$(eval $(call PROGOBJLIST_RULES,$(prog))))



define LINK_RULE
out/$(1): tmp/progobjlist.$(1) $$($(1)_obj:%=tmp/obj/%.o)
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) '[Link %s] %s:\n' '$(3)' '$(1)'
	@$$(PRINTF) '\t%s' $$($(1)_obj)
	@$$(PRINTF) '\n %s\n' '$$($(1)_lib)'
	@$$(LINK.$(2)) -o '$$@' $$($(1)_obj:%=tmp/obj/%.o) $$($(1)_lib)

endef

$(foreach prog,$(CBINARIES),$(eval $(call LINK_RULE,$(prog),c,C)))

$(foreach prog,$(CXXBINARIES),$(eval $(call LINK_RULE,$(prog),cc,C++)))



define LIBOBJLIST_RULES
tmp/libobjlisted.$(1):
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) > 'tmp/libobjlist.$(1)-tmp' '%s\n' $$(sort $$($(1)_mod))

tmp/libobjlist.$(1): | tmp/libobjlisted.$(1)

endef

.IDLE: $(BINARIES:%=tmp/libobjlisted.%)
.IDLE: $(BINARIES:%=tmp/libobjlist.%)

tmp/libobjlist.%: tmp/libobjlisted.%
	@$(call CPCMP,$@-tmp,$@,$* object list)

$(foreach lib,$(LIBRARIES),$(eval $(call LIBOBJLIST_RULES,$(lib))))



define LIB_RULE
out/lib$(1).a: tmp/libobjlist.$(1) $$($(1)_mod:%=tmp/obj/%.o)
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) '[Link %s]:\n' '$(1)'
	@$$(RM) '$$@'
	@$$(AR) r '$$@' $$($(1)_mod:%=tmp/obj/%.o)
	@$$(AR) t '$$@'
	@$$(RANLIB) '$$@'

endef

$(foreach lib,$(LIBRARIES),$(eval $(call LIB_RULE,$(lib))))



tmp/obj/%.o: src/obj/%.c
	@$(MKDIR) '$(@D)'
	@$(PRINTF) '[C] %s\n' '$*'
	@$(COMPILE.c) -o '$@' $(call DEPARGS.c,$@,tmp/obj/$*.mk) 'src/obj/$*.c'

tmp/obj/%.o: src/obj/%.cc
	@$(MKDIR) '$(@D)'
	@$(PRINTF) '[C++] %s\n' '$*'
	@$(COMPILE.cc) -o '$@' $(call DEPARGS.cc,$@,tmp/obj/$*.mk) 'src/obj/$*.cc'

-include $(ALL_OBJECTS:%=tmp/obj/%.mk)

tidy::
	$(FIND) . -name "*~" -delete

clean:: tidy
	$(RM) -r tmp

blank:: clean
	$(RM) -r out


RISCOS_SUFFIXES=c cc h hh s

define RISCOS_APP_SFXDIR_RULES
out/riscos/$$($(1)_appname)/Library/$(2)$(3)/%,fff: src/obj/$(2)%.$(3)
	@$$(MKDIR) -p '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS export] %s <%s%s.%s>\n' '$(1)' '$(2)' '$$*' '$(3)'
	@$$(CP) "$$<" "$$@"

foo::
	@echo out/riscos/$$($(1)_appname)/Library/$(2)$(3)/%,fff: src/obj/$(2)%.$(3)

out/riscos/$$($(1)_appname)/Source/$(2)$(3)/%,fff: src/obj/$(2)%.$(3)
	@$$(MKDIR) -p '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS source] %s %s%s.%s\n' '$(1)' '$(2)' '$$*' '$(3)'
	@$$(CP) "$$<" "$$@"

endef

define RISCOS_APP_RULES
$(1)_appname ?= $(1)

$$(foreach sfx,$$(RISCOS_SUFFIXES),$$(eval $$(call RISCOS_APP_SFXDIR_RULES,$(1),,$$(sfx)))$$(foreach dir,$$(SOURCE_DIRS),$$(eval $$(call RISCOS_APP_SFXDIR_RULES,$(1),$$(dir)/,$$(sfx)))))

out/riscos/$$($(1)_appname)/%,faf: docs/%.html
	@$$(MKDIR) -p '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS HTML] %s %s\n' '$(1)' '$$*'
	@$$(CP) "$$<" "$$@"

out/riscos/$$($(1)_appname)/%,fff: docs/%
	@$$(MKDIR) -p '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS text] %s %s\n' '$(1)' '$$*'
	@$$(CP) "$$<" "$$@"

out/riscos/$$($(1)_appname)/Library/o/%,ffd: out/lib%.a
	@$$(MKDIR) -p '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS export] %s lib%s.a\n' '$(1)' '$$*'
	@$$(CP) "$$<" "$$@"

out/riscos/$$($(1)_appname)/%: src/riscos/$(1)/%
	@$$(MKDIR) -p '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS file] %s %s\n' '$(1)' '$$*'
	@$$(CP) "$$<" "$$@"

out/$(1)-riscos.zip: $$($(1)_app:%=out/riscos/$$($(1)_appname)/%)
	@$$(MKDIR) -p '$$(@D)'
	@$$(PRINTF) '[RISC OS zip] %s\n' '$(1)'
	@$$(RM) '$$@'
	@$$(CD) out/riscos ; \
	$$(ZIP) -rq '$$(abspath $$@)' $$($(1)_app:%=$$($(1)_appname)/%)

endef

$(foreach app,$(riscos_apps),$(eval $(call RISCOS_APP_RULES,$(app))))


# Set this to the comma-separated list of years that should appear in
# the licence.  Do not use characters other than [0-9,] - no spaces.
YEARS=2002-3,2005-6,2012

update-licence:
	$(FIND) . -name ".svn" -prune -or -type f -print0 | $(XARGS) -0 \
	$(SED) -i 's/Copyright (C) [0-9,-]\+  Steven Simpson/Copyright (C) $(YEARS)  Steven Simpson/g'

