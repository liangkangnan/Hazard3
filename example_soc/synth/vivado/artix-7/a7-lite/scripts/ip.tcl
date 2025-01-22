file mkdir $ipdir

update_ip_catalog -rebuild

# MMCM IP

create_ip -name clk_wiz -vendor xilinx.com -library ip -module_name mmcm_main_clk -dir $ipdir -force

set_property -dict [list \
  CONFIG.CLKIN1_JITTER_PS {200.0} \
  CONFIG.CLKOUT1_JITTER {273.316} \
  CONFIG.CLKOUT1_PHASE_ERROR {162.874} \
  CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {12.000} \
  CONFIG.CLK_OUT1_PORT {clk_out_12m} \
  CONFIG.Component_Name {mmcm_main_clk} \
  CONFIG.MMCM_CLKFBOUT_MULT_F {20.250} \
  CONFIG.MMCM_CLKIN1_PERIOD {20.000} \
  CONFIG.MMCM_CLKIN2_PERIOD {10.0} \
  CONFIG.MMCM_CLKOUT0_DIVIDE_F {84.375} \
  CONFIG.PRIM_IN_FREQ {50.000} \
  CONFIG.RESET_PORT {resetn} \
  CONFIG.RESET_TYPE {ACTIVE_LOW} \
] [get_ips mmcm_main_clk]

# The IP core must be marked for OOC generation by setting this property
set_property GENERATE_SYNTH_CHECKPOINT {false} [get_files -all {*.xci}]
set ip_obj [get_ips]
generate_target all $ip_obj
export_ip_user_files -of_objects $ip_obj -no_script -force
