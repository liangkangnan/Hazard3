# SoC integration file

file tinyecp5_soc.v

# CPU + debug components

list ../../hdl/hazard3.f
list ../../hdl/debug/dtm/hazard3_jtag_dtm.f
list ../../hdl/debug/dm/hazard3_dm.f

# RISC-V timer

list peri/hazard3_riscv_timer.f

# Generic SoC components from libfpga

file ../libfpga/common/reset_sync.v

list ../libfpga/peris/uart/uart.f
list ../libfpga/peris/spi_03h_xip/spi_03h_xip.f
list ../libfpga/mem/ahb_cache.f
list ../libfpga/mem/ahb_sync_sram.f

list ../libfpga/busfabric/ahbl_crossbar.f
file ../libfpga/busfabric/ahbl_to_apb.v
file ../libfpga/busfabric/apb_splitter.v

