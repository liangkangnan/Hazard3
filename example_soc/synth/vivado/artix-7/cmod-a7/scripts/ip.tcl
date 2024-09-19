file mkdir $ipdir

update_ip_catalog -rebuild

# MMCM IP

create_ip -name clk_wiz -vendor xilinx.com -library ip -module_name mmcm_main_clk -dir $ipdir -force

set_property -dict [list \
CONFIG.PRIM_IN_FREQ {12.000} \
CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {24.000} \
CONFIG.RESET_TYPE {ACTIVE_LOW} \
CONFIG.RESET_PORT {resetn}] \
[get_ips mmcm_main_clk]

# The IP core must be marked for OOC generation by setting this property
set_property GENERATE_SYNTH_CHECKPOINT {false} [get_files -all {*.xci}]
set ip_obj [get_ips]
generate_target all $ip_obj
export_ip_user_files -of_objects $ip_obj -no_script -force
