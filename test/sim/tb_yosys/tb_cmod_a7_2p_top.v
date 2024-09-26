// 封装实际的顶层模块，因此仿真的顶层模块名对外统一为tb_top

module tb_top (
	input wire        clk,
	input wire        rst_n,

	input  wire       tck,
	input  wire       tms,
	input  wire       tdi,
	output wire       tdo,

	output wire       led
);

    // 实际顶层模块
    fpga_cmod_a7_2p_top u_fpga (
        .clk_12m(clk),
        .tck(tck),
        .tms(tms),
        .tdi(tdi),
        .tdo(tdo),
        .led(led)
    );

endmodule
