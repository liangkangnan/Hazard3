// 封装实际的顶层模块，因此仿真的顶层模块名对外统一为tb_top

module tb_top (
	input wire        clk,
	input wire        rst_n,

	input  wire       tck,
	input  wire       tms,
	input  wire       tdi,
	output wire       tdo,

	output wire       dump_wave_en,

	output wire       led
);

    wire cs;
    wire sck;
    wire mosi;
    wire miso;

    // 实际顶层模块
    fpga_cmod_a7_2p_top u_fpga (
        .clk_12m(clk),
        .tck(tck),
        .tms(tms),
        .tdi(tdi),
        .tdo(tdo),
        .led(led),
        .dump_wave_en(dump_wave_en),
        .spi_cs_n(cs),
        .spi_sck(sck),
        .spi_mosi(mosi),
        .spi_miso(miso)
    );

    sim_flash #(
        .WIDTH(8),
        .DEPTH(1 << 17)
    ) flash (
        .clk(clk),
        .rst_n(rst_n),
        .spi_cs_n(cs),
        .spi_sck(sck),
        .spi_mosi(mosi),
        .spi_miso(miso)
    );

endmodule
