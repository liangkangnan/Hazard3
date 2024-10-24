## 12 MHz Main clock
set_property -dict { PACKAGE_PIN L17   IOSTANDARD LVCMOS33 } [get_ports clk_12m]; #IO_L12P_T1_MRCC_14 Sch=gclk
create_clock -add -name sys_main_clk -period 83.33 -waveform {0 41.66} [get_ports clk_12m]

## LED引脚

set_property -dict { PACKAGE_PIN A17    IOSTANDARD LVCMOS33 } [get_ports led]; #IO_L12N_T1_MRCC_16 Sch=LED1


## 串口引脚

# UART0 TX
set_property -dict { PACKAGE_PIN J18   IOSTANDARD LVCMOS33 } [get_ports uart_tx]; #IO_L7N_T1_D10_14 Sch=uart_rxd_out
# UART0 RX
set_property -dict { PACKAGE_PIN J17   IOSTANDARD LVCMOS33 } [get_ports uart_rx]; #IO_L7P_T1_D09_14 Sch=uart_txd_in


## JTAG引脚

# JTAG TCK引脚
set_property -dict { PACKAGE_PIN W7   IOSTANDARD LVCMOS33 } [get_ports tck]; #IO_L13P_T2_MRCC_34 Sch=pio[46]
# JTAG TMS引脚
set_property -dict { PACKAGE_PIN U8   IOSTANDARD LVCMOS33 } [get_ports tms]; #IO_L14P_T2_SRCC_34 Sch=pio[47]
# JTAG TDI引脚
set_property -dict { PACKAGE_PIN V8   IOSTANDARD LVCMOS33 } [get_ports tdi]; #IO_L14N_T2_SRCC_34 Sch=pio[48]
# JTAG TDO引脚
set_property -dict { PACKAGE_PIN U7   IOSTANDARD LVCMOS33 } [get_ports tdo]; #IO_L19P_T3_34 Sch=pio[45]

#create_clock -add -name jtag_tck -period 200 -waveform {0 100} [get_ports tck]


## Set unused pin pullnone
set_property BITSTREAM.CONFIG.UNUSEDPIN Pullnone [current_design]

set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]  
set_property CONFIG_MODE SPIx4 [current_design] 
set_property BITSTREAM.CONFIG.CONFIGRATE 6 [current_design]

set_property CFGBVS VCCO [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
