# SoC integration file

file a7_lite_2p_soc.v

# CPU + debug components

list ../../../../../hdl/hazard3.f
list ../../../../../hdl/debug/dtm/hazard3_jtag_dtm.f
list ../../../../../hdl/debug/dm/hazard3_dm.f

# RISC-V timer

list ../../../../soc/peri/hazard3_riscv_timer.f

# Generic SoC components from libfpga

file ../../../../libfpga/common/reset_sync.v

list ../../../../libfpga/peris/uart/uart.f
list ../../../../libfpga/peris/spi_qspi_xip/spi_qspi_xip.f
list ../../../../libfpga/peris/pio/pio.f
list ../../../../libfpga/mem/ahb_cache.f
list ../../../../libfpga/mem/ahb_sync_sram.f
list ../../../../libfpga/peris/sysinfo/sysinfo.f
list ../../../../libfpga/peris/peri_reset/peri_reset.f
list ../../../../libfpga/peris/timer/timer.f
list ../../../../libfpga/peris/dma/dma.f
list ../../../../libfpga/peris/crc/crc.f
list ../../../../libfpga/peris/tfpu/tfpu.f
list ../../../../libfpga/peris/sdadc/sdadc.f

list ../../../../libfpga/busfabric/ahbl_crossbar.f
file ../../../../libfpga/busfabric/ahbl_to_apb.v
file ../../../../libfpga/busfabric/apb_splitter.v

include .
