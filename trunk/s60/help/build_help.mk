# ============================================================================
#  Name	 : build_help.mk
#  Part of  : zxEmule
# ============================================================================
#  Name	 : build_help.mk
#  Part of  : zxEmule
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

# build the help from the MAKMAKE step so the header file generated
# will be found by cpp.exe when calculating the dependency information
# in the mmp makefiles.

MAKMAKE : zxEmule_0xE9368B92.hlp
zxEmule_0xE9368B92.hlp : zxEmule.xml zxEmule.cshlp Custom.xml
	cshlpcmp zxEmule.cshlp
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
	copy zxEmule_0xE9368B92.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del zxEmule_0xE9368B92.hlp
	del zxEmule_0xE9368B92.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo zxEmule_0xE9368B92.hlp

FINAL : do_nothing
