set prj_name {hazard3}
set part_fpga {xc7a35tcpg236-1}
set top_module {fpga_cmod_a7_2p_top}

set scriptsdir ./scripts
set constrsdir ./constrs
set outdir ./out
set ipdir ./out/ip


set src_verilog_files [exec ../../../../../scripts/listfiles --relative -f flat fpga_cmod_a7_2p_top.f]
set src_verilog_files_include_dir [exec ../../../../../scripts/listfiles --relative -f flati fpga_cmod_a7_2p_top.f]


# 创建工程(内存模式)
create_project -part $part_fpga -in_memory

# 创建sources_1
if {[get_filesets -quiet sources_1] eq ""} {
    create_fileset -srcset sources_1
}

# 添加verilog文件
add_files -norecurse -fileset sources_1 $src_verilog_files

# 创建constrs_1
if {[get_filesets -quiet constrs_1] eq ""} {
    create_fileset -constrset constrs_1
}

# 添加约束文件
add_files -norecurse -fileset constrs_1 [glob -directory $constrsdir {*.xdc}]

# 创建输出目录
file mkdir $outdir
