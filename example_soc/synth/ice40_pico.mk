include ../project_paths.mk

CHIPNAME=fpga_ice40_pico
DOTF=../fpga/fpga_ice40_pico.f
SYNTH_OPT=-dsp
PNR_OPT=--timing-allow-fail --detailed-timing-report

DEVICE=up5k
PACKAGE=sg48

include $(SCRIPTS)/synth_ice40.mk

bin: bit

rbt: bin
	python3 $(PROJ_ROOT)/tools/bin2rbt.py --binfile $(CHIPNAME).bin --rbtfile $(CHIPNAME).rbt
