// 封装实际的顶层模块，因此仿真的顶层模块名对外统一为tb_top

module tb_top (
	input wire        clk,
	input wire        rst_n,

	input  wire       tck,
	input  wire       tms,
	input  wire       tdi,
	output wire       tdo,

	output wire       dump_wave_en,
	output wire       sim_finish,
	input  wire [31:0]reset_offset,

	output wire       led
);

    wire xip_cs;
    wire xip_sck;
    wire [3:0] spi_din;
    wire [3:0] xip_dout;

    wire [31:0] pio_out_en;
    wire [31:0] pio_out;
    reg  [31:0] pio_in;

    // 实际顶层模块
    fpga_a7_lite_2p_top u_fpga (
        .clk_50m(clk),
        .rst_n(rst_n),
        .tck(tck),
        .tms(tms),
        .tdi(tdi),
        .tdo(tdo),
        .led(led),
        .dump_wave_en(dump_wave_en),
        .sim_finish(sim_finish),
        .reset_offset(reset_offset),
        .xip_cs_n(xip_cs),
        .xip_sck(xip_sck),
        .xip_dout(spi_din),
        .xip_douten(/* unused */),
        .xip_din(xip_dout),
        .pio_in(pio_in),
        .pio_out_en(pio_out_en),
        .pio_out(pio_out)
    );

    sim_flash #(
        .WIDTH(8),
        .DEPTH(1 << 17)
    ) flash (
        .clk(clk),
        .rst_n(rst_n),
        .spi_cs_n(xip_cs),
        .spi_sck(xip_sck),
        .spi_din(spi_din),
        .spi_dout(xip_dout)
    );

    // generate pio_in signals
    always @ (posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            pio_in <= 32'h0;
        end else begin
            pio_in <= ~pio_in;
        end
    end

endmodule
