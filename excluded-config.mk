
list-deps::
	@$(PRINTF) '%s\n' GNU-make ISO-C90 Binodeps

list-opts::
	@$(PRINTF) '%s\n' ISO-C99 ISO-C++11

list-platforms::
	@$(PRINTF) 'riscos'

ddslib_zipdesc=$(cq)iflang($(oq2)eo$(cq2), $(oq2)Antaŭtradukita biblioteko$(cq2), $(oq2)Precompiled library$(cq2))$(oq)


prepackage:: docs/VERSION
