TYPE:=EXE
DEPS:=dbp_tcp_md algo_loader
ifeq ($(SOLA),no)
	LIBS:=
else
	LIBS:=onload_zf
endif
EXE_FLAGS:=-pthread
include $(PROJECT_HOME)/common.mk
