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

CBINARIES += testvstr
CBINARIES += testhash
CBINARIES += testheap
CBINARIES += testree

LIBRARIES += ddslib

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

HEADERS += $(DDSLIB_HEADERS:%=ddslib/%)
HEADERS += $(COMPAT_HEADERS)

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

SOURCES=$(filter-out $(HEADERS),$(patsubst src/obj/%,%,$(wildcard src/obj/*.c src/obj/*.h src/obj/ddslib/*.h)))

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
ddslib_app += $(LIBRARIES:%=Library/o/%,ffd)


all:: $(LIBRARIES:%=out/lib%.a)

install:: install-headers install-libraries

ifneq ($(filter true t y yes on 1,$(call lc,$(ENABLE_RISCOS))),)
install:: install-riscos
all:: out/ddslib-riscos.zip
endif


# Set this to the comma-separated list of years that should appear in
# the licence.  Do not use characters other than [0-9,] - no spaces.
YEARS=2002-3,2005-6,2012

update-licence:
	$(FIND) . -name ".svn" -prune -or -type f -print0 | $(XARGS) -0 \
	$(SED) -i 's/Copyright (C) [0-9,-]\+  Steven Simpson/Copyright (C) $(YEARS)  Steven Simpson/g'


tidy::
	@$(FIND) . -name "*~" -delete


## Generic rules



## Defaults
PREFIX ?= /usr/local
CMP ?= cmp -s
FIND ?= find
AR ?= ar
RANLIB ?= ranlib
CD ?= cd
CP ?= cp
PRINTF ?= printf
MKDIR ?= mkdir -p
ZIP ?= zip
RISCOS_ZIP ?= $(ZIP)
TAR ?= tar
INSTALL ?= install



## Default switches for GCC to generate useful dependencies - We
## select only user headers (not system), we list each dependency in a
## dummy rule to deal with deletions, and targets and the file to
## write the rules in are the first and second arguments.
DEPARGS.c=-MMD -MP -MT '$(1)' -MF '$(2)'
DEPARGS.cc=-MMD -MP -MT '$(1)' -MF '$(2)'



## A command to selectively copy a file if its contents has changed -
## $1 is the source; $2 is the destination; $3 is a label to print if
## the copy occurs.
CPCMP=$(CMP) '$(1)' '$(2)' || ( $(CP) '$(1)' '$(2)' && $(PRINTF) '[Changed]: %s\n' '$(3)' )



## These are the suffixes that we have to swap with a file's leafname
## to get the right order for RISC OS.
RISCOS_SUFFIXES ?= c cc h hh s



## Get a list of directories in source, so we can generate non-trivial
## RISC OS rules for each one.
SOURCE_DIRS:=$(shell $(FIND) src/obj -type d -printf '%P')



## Get a complete list of binary executables and the object files that
## make them up.
BINARIES += $(CBINARIES)
BINARIES += $(CXXBINARIES)
ALL_OBJECTS=$(sort $(foreach prog,$(BINARIES),$($(prog)_obj)) $(foreach lib,$(LIBRARIES),$($(lib)_mod)))



## Just use the internal RISC OS app name if an external name isn't
## defined.
define RISCOS_APP_DEFS
$(1)_appname ?= $(1)

endef
$(foreach app,$(riscos_apps),$(eval $(call RISCOS_APP_DEFS,$(app))))





## Make sure we can detect changes to the list of object files that
## make up each binary executable.
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



## Define rules for building each binary executable.
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



## Make sure we can detect changes to the list of object files that
## make up each binary library.
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



## Define rules for building each binary library.
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



## Some diagnostic rules
list-c-opts:
	@$(PRINTF) '%s\n' '$(CPPFLAGS) $(CFLAGS)'

list-cc-opts:
	@$(PRINTF) '%s\n' '$(CPPFLAGS) $(CXXFLAGS)'

show-prog-%:
	@$(PRINTF) 'Binary executable %s:\n' '$*'
	@$(PRINTF) '  Libraries: %s\n' '$($*_lib)'
	@$(PRINTF) '  Object: %s\n' $($*_obj)

show-lib-%:
	@$(PRINTF) 'Binary library %s:\n' '$*'
	@$(PRINTF) '  Object: %s\n' $($*_mod)

show-progs: $(BINARIES:%=show-prog-%)
show-libs: $(LIBRARIES:%=show-lib-%)

.PHONY: list-c-opts list-cc-opts




## When C and C++ files are compiled, ensure we keep track of which
## files were included, and use these generated rules on the next
## invocation of 'make'.
tmp/obj/%.o: src/obj/%.c
	@$(MKDIR) '$(@D)'
	@$(PRINTF) '[C] %s\n' '$*'
	@$(COMPILE.c) -o '$@' $(call DEPARGS.c,$@,tmp/obj/$*.mk) 'src/obj/$*.c'

tmp/obj/%.o: src/obj/%.cc
	@$(MKDIR) '$(@D)'
	@$(PRINTF) '[C++] %s\n' '$*'
	@$(COMPILE.cc) -o '$@' $(call DEPARGS.cc,$@,tmp/obj/$*.mk) 'src/obj/$*.cc'

-include $(ALL_OBJECTS:%=tmp/obj/%.mk)



## Some basic housekeeping
tidy::

clean:: tidy
	@$(RM) -r tmp

blank:: clean
	@$(RM) -r out



## Source files of the form foo/bar.h appear on RISC OS filesystems as
## foo.h.bar (where . is a directory separator).  Knowing the list of
## source directories, we can generate rules for each one to copy the
## conventionally named files to RISC OS form.  We also transpose
## . and /, and add the type suffix, because we're still looking at
## the files from a Unix point of view.

define RISCOS_APP_SFXDIR_RULES
out/riscos/$$($(1)_appname)/Library/$(2)$(3)/%,fff: src/obj/$(2)%.$(3)
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS export] %s <%s%s.%s>\n' '$(1)' '$(2)' '$$*' '$(3)'
	@$$(CP) "$$<" "$$@"

out/riscos/$$($(1)_appname)/Source/$(2)$(3)/%,fff: src/obj/$(2)%.$(3)
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS source] %s %s%s.%s\n' '$(1)' '$(2)' '$$*' '$(3)'
	@$$(CP) "$$<" "$$@"

endef

define RISCOS_APP_RULES
$$(foreach sfx,$$(RISCOS_SUFFIXES),$$(eval $$(call RISCOS_APP_SFXDIR_RULES,$(1),,$$(sfx)))$$(foreach dir,$$(SOURCE_DIRS),$$(eval $$(call RISCOS_APP_SFXDIR_RULES,$(1),$$(dir)/,$$(sfx)))))

out/riscos/$$($(1)_appname)/%,faf: docs/%.html
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS HTML] %s %s\n' '$(1)' '$$*'
	@$$(CP) "$$<" "$$@"

out/riscos/$$($(1)_appname)/%,fff: docs/%
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS text] %s %s\n' '$(1)' '$$*'
	@$$(CP) "$$<" "$$@"

out/riscos/$$($(1)_appname)/Library/o/%,ffd: out/lib%.a
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS export] %s lib%s.a\n' '$(1)' '$$*'
	@$$(CP) "$$<" "$$@"

out/riscos/$$($(1)_appname)/%: src/riscos/$(1)/%
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) '[Copy RISC OS file] %s %s\n' '$(1)' '$$*'
	@$$(CP) "$$<" "$$@"

out/$(1)-riscos.zip: $$($(1)_app:%=out/riscos/$$($(1)_appname)/%)
	@$$(MKDIR) '$$(@D)'
	@$$(PRINTF) '[RISC OS zip] %s\n' '$(1)'
	@$$(RM) '$$@'
	@$$(CD) out/riscos ; \
	$$(RISCOS_ZIP) -rq '$$(abspath $$@)' $$($(1)_app:%=$$($(1)_appname)/%)

endef

$(foreach app,$(riscos_apps),$(eval $(call RISCOS_APP_RULES,$(app))))


## Standard installation rules
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

## Find all the directories that contain headers to be installed.
## Headers that are not listed under directories yield "./".
HEADER_DIRS=$(sort $(dir $(HEADERS)))

## Rename the headers so that those not listed under directories are
## prefixed with "./", so that we can match them.  For example
## "foo/bar.h" remains unchanged, but "bar.h" becomes "./bar.h".
ALT_HEADERS=$(join $(dir $(HEADERS)),$(notdir $(HEADERS)))

## Get the list of headers under a directory.  Use "./" for the top
## level.  Use "foo/" for everything else.
HDRSUNDIR=$(patsubst $(1)%,%,$(filter $(1)%,$(ALT_HEADERS)))

## Get the list of headers that are direct descendants of a directory.
## We take the list of headers *under* the directory, and then remove
## only those that have no directory component.
HDRSINDIR=$(filter $(notdir $(call HDRSUNDIR,$(1))),$(call HDRSUNDIR,$(1)))

## Install headers that are direct descendants of a directory.  The
## directory name must have a trailing slash.  Use "./" for the top
## level.
define HEADER_INSTALL_COMMANDS
$(INSTALL) -d '$(PREFIX)/include/$1'
$(INSTALL) -m 0644 $(foreach file,$(call HDRSINDIR,$1),src/obj/$1$(file)) \
  '$(PREFIX)/include/$1'

endef

## Install the headers.  Everything in $(HEADERS) from src/obj/ is
## copied to $(PREFIX)/include, preserving the hierarchy.
install-headers:
	@$(PRINTF) 'Installing headers in %s:\n' '$(PREFIX)/include'
	@$(PRINTF) '\t<%s>\n' $(HEADERS)
	@$(foreach dir,$(HEADER_DIRS),$(call HEADER_INSTALL_COMMANDS,$(dir)))
