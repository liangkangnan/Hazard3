include ../project_paths.mk

CHIPNAME=fpga_tinyecp5
TOP=fpga_tinyecp5
DOTF=../fpga/fpga_tinyecp5.f

SYNTH_OPT=-abc9
PNR_OPT=--timing-allow-fail

DEVICE=25k
PACKAGE=CABGA256

DEVICE_IDCODE=0x41111043

include $(SCRIPTS)/synth_ecp5.mk

#prog: bit
#	ujprog $(CHIPNAME).bit

#flash: bit
#	ujprog -j flash $(CHIPNAME).bit
