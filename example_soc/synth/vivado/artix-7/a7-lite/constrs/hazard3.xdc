################### 50 MHz Main clock ###################

set_property -dict { PACKAGE_PIN J19   IOSTANDARD LVCMOS33 } [get_ports clk_50m]
create_clock -add -name sys_main_clk -period 20.00 -waveform {0 10} [get_ports clk_50m]

################### LED引脚 ###################

set_property -dict { PACKAGE_PIN M18   IOSTANDARD LVCMOS33 } [get_ports led]; # Sch=LED1

################### 串口引脚 ###################

set_property -dict { PACKAGE_PIN F13   IOSTANDARD LVCMOS33 } [get_ports uart_tx]; # Sch=GPIO1_0P
set_property -dict { PACKAGE_PIN F14   IOSTANDARD LVCMOS33 } [get_ports uart_rx]; # Sch=GPIO1_0N

################### Reset引脚 ###################

set_property -dict { PACKAGE_PIN L18  IOSTANDARD LVCMOS33 } [get_ports rst_n]; # Sch=K3

################### PIO引脚 ###################

set_property -dict { PACKAGE_PIN W21  IOSTANDARD LVCMOS33 } [get_ports { pio[0] }];  # Sch=GPIO2_0P
set_property -dict { PACKAGE_PIN W22  IOSTANDARD LVCMOS33 } [get_ports { pio[1] }];  # Sch=GPIO2_0N
set_property -dict { PACKAGE_PIN N17  IOSTANDARD LVCMOS33 } [get_ports { pio[2] }];  # Sch=GPIO2_1P
set_property -dict { PACKAGE_PIN P17  IOSTANDARD LVCMOS33 } [get_ports { pio[3] }];  # Sch=GPIO2_1N
set_property -dict { PACKAGE_PIN P19  IOSTANDARD LVCMOS33 } [get_ports { pio[4] }];  # Sch=GPIO2_2P
set_property -dict { PACKAGE_PIN R19  IOSTANDARD LVCMOS33 } [get_ports { pio[5] }];  # Sch=GPIO2_2N
set_property -dict { PACKAGE_PIN R18  IOSTANDARD LVCMOS33 } [get_ports { pio[6] }];  # Sch=GPIO2_3P
set_property -dict { PACKAGE_PIN T18  IOSTANDARD LVCMOS33 } [get_ports { pio[7] }];  # Sch=GPIO2_3N
set_property -dict { PACKAGE_PIN T21  IOSTANDARD LVCMOS33 } [get_ports { pio[8] }];  # Sch=GPIO2_4P
set_property -dict { PACKAGE_PIN U21  IOSTANDARD LVCMOS33 } [get_ports { pio[9] }];  # Sch=GPIO2_4N
set_property -dict { PACKAGE_PIN Y21  IOSTANDARD LVCMOS33 } [get_ports { pio[10] }]; # Sch=GPIO2_6P
set_property -dict { PACKAGE_PIN Y22  IOSTANDARD LVCMOS33 } [get_ports { pio[11] }]; # Sch=GPIO2_6N
set_property -dict { PACKAGE_PIN AA20 IOSTANDARD LVCMOS33 } [get_ports { pio[12] }]; # Sch=GPIO2_7P
set_property -dict { PACKAGE_PIN AA21 IOSTANDARD LVCMOS33 } [get_ports { pio[13] }]; # Sch=GPIO2_7N
set_property -dict { PACKAGE_PIN AB21 IOSTANDARD LVCMOS33 } [get_ports { pio[14] }]; # Sch=GPIO2_8P
set_property -dict { PACKAGE_PIN AB22 IOSTANDARD LVCMOS33 } [get_ports { pio[15] }]; # Sch=GPIO2_8N
set_property -dict { PACKAGE_PIN AA19 IOSTANDARD LVCMOS33 } [get_ports { pio[16] }]; # Sch=GPIO2_9P
set_property -dict { PACKAGE_PIN AB20 IOSTANDARD LVCMOS33 } [get_ports { pio[17] }]; # Sch=GPIO2_9N
set_property -dict { PACKAGE_PIN U20  IOSTANDARD LVCMOS33 } [get_ports { pio[18] }]; # Sch=GPIO2_10P
set_property -dict { PACKAGE_PIN V20  IOSTANDARD LVCMOS33 } [get_ports { pio[19] }]; # Sch=GPIO2_10N
set_property -dict { PACKAGE_PIN Y18  IOSTANDARD LVCMOS33 } [get_ports { pio[20] }]; # Sch=GPIO2_11P
set_property -dict { PACKAGE_PIN Y19  IOSTANDARD LVCMOS33 } [get_ports { pio[21] }]; # Sch=GPIO2_11N
set_property -dict { PACKAGE_PIN W19  IOSTANDARD LVCMOS33 } [get_ports { pio[22] }]; # Sch=GPIO2_12P
set_property -dict { PACKAGE_PIN W20  IOSTANDARD LVCMOS33 } [get_ports { pio[23] }]; # Sch=GPIO2_12N
set_property -dict { PACKAGE_PIN AA18 IOSTANDARD LVCMOS33 } [get_ports { pio[24] }]; # Sch=GPIO2_13P
set_property -dict { PACKAGE_PIN AB18 IOSTANDARD LVCMOS33 } [get_ports { pio[25] }]; # Sch=GPIO2_13N
set_property -dict { PACKAGE_PIN V18  IOSTANDARD LVCMOS33 } [get_ports { pio[26] }]; # Sch=GPIO2_14P
set_property -dict { PACKAGE_PIN V19  IOSTANDARD LVCMOS33 } [get_ports { pio[27] }]; # Sch=GPIO2_14N
set_property -dict { PACKAGE_PIN V17  IOSTANDARD LVCMOS33 } [get_ports { pio[28] }]; # Sch=GPIO2_15P
set_property -dict { PACKAGE_PIN W17  IOSTANDARD LVCMOS33 } [get_ports { pio[29] }]; # Sch=GPIO2_15N
set_property -dict { PACKAGE_PIN U17  IOSTANDARD LVCMOS33 } [get_ports { pio[30] }]; # Sch=GPIO2_16P
set_property -dict { PACKAGE_PIN U18  IOSTANDARD LVCMOS33 } [get_ports { pio[31] }]; # Sch=GPIO2_16N

################### XIP引脚 ###################

set_property -dict { PACKAGE_PIN E13  IOSTANDARD LVCMOS33 } [get_ports xip_cs_n]      ; # Sch=GPIO1_1P
set_property -dict { PACKAGE_PIN E14  IOSTANDARD LVCMOS33 } [get_ports xip_sck]       ; # Sch=GPIO1_1N
set_property -dict { PACKAGE_PIN D14  IOSTANDARD LVCMOS33 } [get_ports { xip_io[0] }] ; # Sch=GPIO1_2P
set_property -dict { PACKAGE_PIN D15  IOSTANDARD LVCMOS33 } [get_ports { xip_io[1] }] ; # Sch=GPIO1_2N
set_property -dict { PACKAGE_PIN E16  IOSTANDARD LVCMOS33 } [get_ports { xip_io[2] }] ; # Sch=GPIO1_3P
set_property -dict { PACKAGE_PIN D16  IOSTANDARD LVCMOS33 } [get_ports { xip_io[3] }] ; # Sch=GPIO1_3N

################### JTAG引脚 ###################

set_property -dict { PACKAGE_PIN C18   IOSTANDARD LVCMOS33 } [get_ports tck] ; # Sch=GPIO1_16P
set_property -dict { PACKAGE_PIN C19   IOSTANDARD LVCMOS33 } [get_ports tms] ; # Sch=GPIO1_16N
set_property -dict { PACKAGE_PIN E19   IOSTANDARD LVCMOS33 } [get_ports tdi] ; # Sch=GPIO1_15P
set_property -dict { PACKAGE_PIN D19   IOSTANDARD LVCMOS33 } [get_ports tdo] ; # Sch=GPIO1_15N

#create_clock -add -name jtag_tck -period 200 -waveform {0 100} [get_ports tck]

################### Set unused pin pullnone ###################

set_property BITSTREAM.CONFIG.UNUSEDPIN Pullnone [current_design]

set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]  
set_property CONFIG_MODE SPIx4 [current_design] 
set_property BITSTREAM.CONFIG.CONFIGRATE 6 [current_design]

set_property CFGBVS VCCO [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
