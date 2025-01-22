# ----------------------------------------------------------------------------
# 
#  Created With Xilinx Vavado 2021.1 
#   Date : 2022.09.01
# 
#  This design is the property of MicroPhase.  Publication of this
#  design is not authorized without written consent from MicroPhase.
# --------------------------------------------------------------------
#           
#               MicroPhase Technologies Inc
#               Shanghai, China
#
#               web: http://www.microphase.cn   
#               email: support@microphase.cn
# --------------------------------------------------------------------
#  DiEEPROM_I2C_SCLaimer:
#     MicroPhase, Inc. makes no warranty for the use of this code or design.
#     This code is provided  "As Is". MicroPhase, Inc assumes no responsibility
#     for any errors, which may appear in this code, nor does it make a commitment
#     to update the information contained herein. MicroPhase, Inc specifically
#     diEEPROM_I2C_SCLaims any implied warranties of fitness for a particular purpose.
#                      Copyright(c) 2022 MicroPhase, Inc.
#                              All rights reserved.
# 
# ----------------------------------------------------------------------------
#################时钟信号和复位信号##########################
set_property PACKAGE_PIN J19 [get_ports CLK_50M]
set_property PACKAGE_PIN L18 [get_ports RESET]

set_property IOSTANDARD LVCMOS33 [get_ports CLK_50M]
set_property IOSTANDARD LVCMOS33 [get_ports RESET]
#################按键信号##########################
set_property PACKAGE_PIN AA1 [get_ports KEY1]
set_property PACKAGE_PIN W1 [get_ports KEY2]

set_property IOSTANDARD LVCMOS33 [get_ports KEY1]
set_property IOSTANDARD LVCMOS33 [get_ports KEY2]
#################LED#################
set_property PACKAGE_PIN M18 [get_ports LED1]
set_property PACKAGE_PIN N18 [get_ports LED2]

set_property IOSTANDARD LVCMOS33 [get_ports LED1]
set_property IOSTANDARD LVCMOS33 [get_ports LED2]
#################HDMI#################
set_property PACKAGE_PIN L19 [get_ports HDMI1_CLK_P]
set_property PACKAGE_PIN K21 [get_ports HDMI1_D0_P]
set_property PACKAGE_PIN J20 [get_ports HDMI1_D1_P]
set_property PACKAGE_PIN G17 [get_ports HDMI1_D2_P]
set_property PACKAGE_PIN H15 [get_ports HDMI1_HPD_CON]

set_property IOSTANDARD TMDS_33  [get_ports HDMI1_CLK_P]
set_property IOSTANDARD TMDS_33  [get_ports HDMI1_D0_P]
set_property IOSTANDARD TMDS_33  [get_ports HDMI1_D1_P]
set_property IOSTANDARD TMDS_33  [get_ports HDMI1_D2_P]
set_property IOSTANDARD LVCMOS33 [get_ports HDMI1_HPD_CON]
#################UART#################
set_property PACKAGE_PIN  U2      [get_ports UART_RX]
set_property PACKAGE_PIN  V2      [get_ports UART_TX]

set_property IOSTANDARD   LVCMOS33 [get_ports UART_RX]
set_property IOSTANDARD   LVCMOS33 [get_ports UART_TX]
#################EEPROM#################
set_property PACKAGE_PIN  J22    [get_ports EEPROM_I2C_SCL]
set_property PACKAGE_PIN  H22    [get_ports EEPROM_I2C_SDA]

set_property IOSTANDARD LVCMOS33 [get_ports EEPROM_I2C_SCL]     
set_property IOSTANDARD LVCMOS33 [get_ports EEPROM_I2C_SDA]   
#################sd card#################
set_property PACKAGE_PIN   U7    [get_ports SD_CLK]
set_property PACKAGE_PIN   AA8   [get_ports SD_CMD]
set_property PACKAGE_PIN   W9    [get_ports SD_DATA0]
set_property PACKAGE_PIN   Y9    [get_ports SD_DATA1]
set_property PACKAGE_PIN   Y7    [get_ports SD_DATA2]
set_property PACKAGE_PIN   Y8    [get_ports SD_DATA3]

set_property IOSTANDARD LVCMOS33 [get_ports SD_CLK]   
set_property IOSTANDARD LVCMOS33 [get_ports SD_CMD]
set_property IOSTANDARD LVCMOS33 [get_ports SD_DATA0]   
set_property IOSTANDARD LVCMOS33 [get_ports SD_DATA1]
set_property IOSTANDARD LVCMOS33 [get_ports SD_DATA2]   
set_property IOSTANDARD LVCMOS33 [get_ports SD_DATA3]

########################ethernet port######################
set_property PACKAGE_PIN N22 [get_ports ETH_nRST]
set_property PACKAGE_PIN M22 [get_ports ETH_MDC]
set_property PACKAGE_PIN M20 [get_ports ETH_MDIO]
set_property PACKAGE_PIN K18 [get_ports ETH_RXCK]
set_property PACKAGE_PIN K19 [get_ports ETH_RXCTL]
set_property PACKAGE_PIN M16 [get_ports {ETH_RXD[3]}]
set_property PACKAGE_PIN L16 [get_ports {ETH_RXD[2]}]
set_property PACKAGE_PIN M15 [get_ports {ETH_RXD[1]}]
set_property PACKAGE_PIN L14 [get_ports {ETH_RXD[0]}]
set_property PACKAGE_PIN K17 [get_ports ETH_TXCK]
set_property PACKAGE_PIN N20 [get_ports ETH_TXCTL]
set_property PACKAGE_PIN M13 [get_ports {ETH_TXD[3]}]
set_property PACKAGE_PIN L13 [get_ports {ETH_TXD[2]}]
set_property PACKAGE_PIN L15 [get_ports {ETH_TXD[1]}]
set_property PACKAGE_PIN K16 [get_ports {ETH_TXD[0]}]

set_property IOSTANDARD LVCMOS33 [get_ports ETH_nRST]
set_property IOSTANDARD LVCMOS33 [get_ports ETH_MDC]
set_property IOSTANDARD LVCMOS33 [get_ports ETH_MDIO]
set_property IOSTANDARD LVCMOS33 [get_ports ETH_RXCK]
set_property IOSTANDARD LVCMOS33 [get_ports ETH_RXCTL]
set_property IOSTANDARD LVCMOS33 [get_ports {ETH_RXD[*]}]
set_property IOSTANDARD LVCMOS33 [get_ports ETH_TXCK]
set_property IOSTANDARD LVCMOS33 [get_ports ETH_TXCTL]
set_property IOSTANDARD LVCMOS33 [get_ports {ETH_TXD[*]}]
