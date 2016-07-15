
list-deps::
	@echo GNU-make ISO-C90 Binodeps

list-opts::
	@echo ISO-C99 ISO-C++11

list-platforms::
	@echo linux riscos

ddslib_zipdesc=$(cq)iflang($(oq2)eo$(cq2), $(oq2)Antaŭtradukita biblioteko$(cq2), $(oq2)Precompiled library$(cq2))$(oq)


prepackage:: docs/VERSION
